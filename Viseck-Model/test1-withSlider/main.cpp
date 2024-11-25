#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

// Constants
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int NUM_BIRDS = 100;
const float TWO_PI = 6.28318530718f;

// Parameters
float bird_speed = 2.0f;
float alignment_radius = 50.0f;
float noise_variance = 0.2f;
float wiggle_intensity = 0.05f;
float bird_radius = 5.0f;

// Function to apply periodic boundary conditions
float applyPeriodicBoundary(float value, float max) {
  if (value < 0) return max + value;
  if (value >= max) return value - max;
  return value;
}

// Vector utility functions
sf::Vector2f normalize(const sf::Vector2f& v) {
  float magnitude = std::sqrt(v.x * v.x + v.y * v.y);
  return (magnitude == 0) ? sf::Vector2f(0, 0) : sf::Vector2f(v.x / magnitude, v.y / magnitude);
}

float vectorMagnitude(const sf::Vector2f& v) {
  return std::sqrt(v.x * v.x + v.y * v.y);
}

// Bird structure
struct Bird {
  sf::Vector2f position;
  float heading; // Angle in radians

  Bird(float x, float y, float angle)
    : position(x, y), heading(angle) {}

  sf::Vector2f getVelocity() const {
    return sf::Vector2f(bird_speed * std::cos(heading), bird_speed * std::sin(heading));
  }
};

// Generate a random noise value for direction change
float generateNoise() {
  return (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * noise_variance * TWO_PI;
}

// Generate a small wiggle adjustment to make the movement smoother
float generateWiggle() {
  return (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * wiggle_intensity * TWO_PI;
}

// Calculate bird color based on the heading
sf::Color calculateColor(float heading) {
  // Normalize heading to [0, 1] for color mapping (0 to 2π range)
  float normalizedHeading = (heading + TWO_PI) / TWO_PI;
  int red = static_cast<int>(255 * normalizedHeading);
  int blue = static_cast<int>(255 * (1 - normalizedHeading));
  return sf::Color(red, 0, blue);
}

// Simple slider class using SFML rectangles
class Slider {
  public:
    Slider(float x, float y, float width, float minVal, float maxVal, float& value, const std::string& labelText)
      : position(x, y), sliderWidth(width), minValue(minVal), maxValue(maxVal), currentValue(value), isDragging(false) {
        // Background bar
        bar.setSize(sf::Vector2f(sliderWidth, 5));
        bar.setFillColor(sf::Color::White);
        bar.setPosition(position);

        // Handle
        handle.setSize(sf::Vector2f(10, 20));
        handle.setFillColor(sf::Color::Red);
        updateHandlePosition();

        // Label
        if (!font.loadFromFile("ariali.ttf")) {
          // Error loading font
        }
        label.setFont(font);
        label.setString(labelText);
        label.setCharacterSize(12);
        label.setFillColor(sf::Color::White);
        label.setPosition(position.x, position.y - 20);
      }

    // Update the slider position based on mouse events
    void update(const sf::RenderWindow& window, const sf::Event& event) {
      if (event.type == sf::Event::MouseButtonPressed) {
        if (handle.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
          isDragging = true;
        }
      } else if (event.type == sf::Event::MouseButtonReleased) {
        isDragging = false;
      }

      if (isDragging && event.type == sf::Event::MouseMoved) {
        float mouseX = static_cast<float>(event.mouseMove.x);
        mouseX = std::clamp(mouseX, position.x, position.x + sliderWidth);

        // Update current value based on handle position
        float percentage = (mouseX - position.x) / sliderWidth;
        currentValue = minValue + percentage * (maxValue - minValue);

        // Update handle position
        updateHandlePosition();
      }
    }

    // Draw the slider and label
    void draw(sf::RenderWindow& window) const {
      window.draw(bar);
      window.draw(handle);
      window.draw(label);
    }

  private:
    sf::Vector2f position;
    float sliderWidth;
    float minValue, maxValue;
    float& currentValue;
    bool isDragging;
    sf::RectangleShape bar;
    sf::RectangleShape handle;
    sf::Text label;
    sf::Font font;

    void updateHandlePosition() {
      float percentage = (currentValue - minValue) / (maxValue - minValue);
      handle.setPosition(position.x + percentage * sliderWidth - handle.getSize().x / 2, position.y - handle.getSize().y / 2);
    }
};

int main() {
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  // Initialize the window
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Vicsek Model with SFML Sliders");
  window.setFramerateLimit(60);

  // Initialize the flock of birds
  std::vector<Bird> birds;
  for (int i = 0; i < NUM_BIRDS; ++i) {
    float x = static_cast<float>(std::rand() % WINDOW_WIDTH);
    float y = static_cast<float>(std::rand() % WINDOW_HEIGHT);
    float angle = static_cast<float>(std::rand()) / RAND_MAX * TWO_PI;
    birds.emplace_back(x, y, angle);
  }

  // Sliders for simulation parameters
  Slider speedSlider(10, 10, 200, 0.1f, 10.0f, bird_speed, "Speed");
  Slider alignmentRadiusSlider(10, 50, 200, 10.0f, 200.0f, alignment_radius, "Alignment Radius");
  Slider wiggleSlider(10, 90, 200, 0.0f, 0.5f, wiggle_intensity, "Wiggle Intensity");
  Slider noiseSlider(10, 130, 200, 0.0f, 1.0f, noise_variance, "Noise Intensity");
  Slider radiusSlider(10, 170, 200, 1.0f, 10.0f, bird_radius, "Bird Radius");

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      // Update sliders based on mouse input
      speedSlider.update(window, event);
      alignmentRadiusSlider.update(window, event);
      wiggleSlider.update(window, event);
      noiseSlider.update(window, event);
      radiusSlider.update(window, event);
    }

    // Update birds' headings
    std::vector<float> newHeadings(NUM_BIRDS);
    for (size_t i = 0; i < birds.size(); ++i) {
      sf::Vector2f averageDirection(0, 0);
      int neighborCount = 0;

      // Calculate the average direction of neighbors
      for (size_t j = 0; j < birds.size(); ++j) {
        if (i != j) {
          sf::Vector2f diff = birds[j].position - birds[i].position;
          // Apply periodic boundary conditions
          if (std::abs(diff.x) > WINDOW_WIDTH / 2) diff.x = -diff.x;
          if (std::abs(diff.y) > WINDOW_HEIGHT / 2) diff.y = -diff.y;

          if (vectorMagnitude(diff) < alignment_radius) {
            averageDirection += normalize(birds[j].getVelocity());
            neighborCount++;
          }
        }
      }

      // Compute new heading with continuous wiggle
      if (neighborCount > 0) {
        averageDirection /= static_cast<float>(neighborCount);
        float angle = std::atan2(averageDirection.y, averageDirection.x);
        angle += generateNoise(); // Add random noise
        angle += generateWiggle(); // Add continuous wiggle
        newHeadings[i] = angle;
      } else {
        newHeadings[i] = birds[i].heading + generateNoise() + generateWiggle();
      }
    }

    // Update positions and headings
    for (size_t i = 0; i < birds.size(); ++i) {
      birds[i].heading = newHeadings[i];
      sf::Vector2f velocity = birds[i].getVelocity();
      birds[i].position += velocity;

      // Apply periodic boundary conditions
      birds[i].position.x = applyPeriodicBoundary(birds[i].position.x, WINDOW_WIDTH);
      birds[i].position.y = applyPeriodicBoundary(birds[i].position.y, WINDOW_HEIGHT);
    }

    // Render
    window.clear(sf::Color::Black);
    for (const auto& bird : birds) {
      sf::CircleShape shape(bird_radius);
      shape.setPosition(bird.position.x - bird_radius, bird.position.y - bird_radius);
      shape.setFillColor(calculateColor(bird.heading));
      window.draw(shape);
    }

    // Draw sliders
    speedSlider.draw(window);
    alignmentRadiusSlider.draw(window);
    wiggleSlider.draw(window);
    noiseSlider.draw(window);
    radiusSlider.draw(window);

    window.display();
  }

  return 0;
}

