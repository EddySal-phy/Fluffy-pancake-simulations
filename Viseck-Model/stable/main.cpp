#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Constants
const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
const int NUM_BIRDS = 500;
const float RADIUS = 2.0f;
const float SPEED = 0.5f;
const float ALIGNMENT_RADIUS = 20.0f;
const float NOISE_VARIANCE = 0.2f; //  Noise for wiggle
const float WIGGLE_INTENSITY = 0.05f; // Wiggle intensity (smaller value for smooth movement)
const float TWO_PI = 6.28318530718f;

// Function to apply periodic boundary conditions.
float applyPeriodicBoundary(float value, float max) {
  if (value < 0) return max + value;
  if (value >= max) return value - max;
  return value;
}

sf::Vector2f normalize(const sf::Vector2f& v) {
  float magnitude = std::sqrt(v.x * v.x + v.y * v.y);
  return (magnitude == 0) ? sf::Vector2f(0, 0) : sf::Vector2f(v.x / magnitude, v.y / magnitude);
}

float vectorMagnitude(const sf::Vector2f& v) {
  return std::sqrt(v.x * v.x + v.y * v.y);
}

struct Bird {
  sf::Vector2f position;
  float heading; // Angle in radians

  Bird(float x, float y, float angle)
    : position(x, y), heading(angle) {}

  sf::Vector2f getVelocity() const {
    return sf::Vector2f(SPEED * std::cos(heading), SPEED * std::sin(heading));
  }
};

// Generate a random noise value for direction change
float generateNoise() {
  return (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * NOISE_VARIANCE * TWO_PI;
}

// Generate a small wiggle adjustment to make the movement smoother
float generateWiggle() {
  return (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * WIGGLE_INTENSITY * TWO_PI;
}

// Calculate bird color based on the heading
sf::Color calculateColor(float heading) {
  // Normalize heading to [0, 1] for color mapping (0 to 2π range)
  float normalizedHeading = (heading + TWO_PI) / TWO_PI;
  int red = static_cast<int>(255 * normalizedHeading);
  int blue = static_cast<int>(255 * (1 - normalizedHeading));
  return sf::Color(red, 0, blue);
}

int main() {
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Vicsek Model - Flock Simulation");

  // Initialize the flock
  std::vector<Bird> birds;
  for (int i = 0; i < NUM_BIRDS; ++i) {
    float x = static_cast<float>(std::rand() % WINDOW_WIDTH);
    float y = static_cast<float>(std::rand() % WINDOW_HEIGHT);
    float angle = static_cast<float>(std::rand()) / RAND_MAX * TWO_PI;
    birds.emplace_back(x, y, angle);
  }

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // Update headings
    std::vector<float> newHeadings(NUM_BIRDS);
    for (size_t i = 0; i < birds.size(); ++i) {
      sf::Vector2f averageDirection(0, 0);
      int neighborCount = 0;

      // compute the average direction of neighbors
      for (size_t j = 0; j < birds.size(); ++j) {
        if (i != j) {
          sf::Vector2f diff = birds[j].position - birds[i].position;
          // Periodic boundary conditions
          if (std::abs(diff.x) > WINDOW_WIDTH / 2) diff.x = -diff.x;
          if (std::abs(diff.y) > WINDOW_HEIGHT / 2) diff.y = -diff.y;

          if (vectorMagnitude(diff) < ALIGNMENT_RADIUS) {
            averageDirection += normalize(birds[j].getVelocity());
            neighborCount++;
          }
        }
      }

      // Compute new heading with  wiggle
      if (neighborCount > 0) {
        averageDirection /= static_cast<float>(neighborCount);
        float angle = std::atan2(averageDirection.y, averageDirection.x);
        angle += generateNoise(); // Add some noise
        angle += generateWiggle(); // Add  wiggle
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
      sf::CircleShape shape(RADIUS);
      shape.setFillColor(calculateColor(bird.heading));
      shape.setPosition(bird.position.x, bird.position.y);
      shape.setOrigin(RADIUS, RADIUS);  // Center the shape
      window.draw(shape);
    }

    window.display();
  }

  return 0;
}

