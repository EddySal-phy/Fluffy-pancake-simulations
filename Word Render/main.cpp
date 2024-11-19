#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

// Function to encode a string using Caesar Cipher
std::string caesarCipherEncode(const std::string &text, int shift) {
    std::string result = "";

    // Loop through each character in the string
    for (char c : text) {
        if (std::isalpha(c)) {
            // Check if the character is uppercase or lowercase
            char offset = std::isupper(c) ? 'A' : 'a';
            // Shift the character and wrap around the alphabet
            result += (c - offset + shift) % 26 + offset;
        } else {
            // Non-alphabetic characters are unchanged
            result += c;
        }
    }
    return result;
}

// Function to decode a string using Caesar Cipher
std::string caesarCipherDecode(const std::string &text, int shift) {
    // Decoding is just encoding with a negative shift
    return caesarCipherEncode(text, 26 - (shift % 26));
}


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PARTICLE_COUNT = 3000;
const float TEXT_CONVERGE_TIME = 1.0f; // Time in seconds when particles start converging
const float PARTICLE_SPEED = 200.0f;    // Particle speed
const float VIBRATION_AMPLITUDE = 10.0f; // Amplitude of initial vibration

// Cypher text
const std::string TEXT = "Parabens, Ana!";
std::string ENCODED_TEXT = caesarCipherEncode(TEXT, 2);
std::string DECODED_TEXT = caesarCipherDecode(ENCODED_TEXT, 2);

// Target text
const std::string TARGET_TEXT = DECODED_TEXT ;
const int FONT_SIZE = 80;
const std::string HEART_IMAGE_FILE = "heart.png";


// Helper function to get a random float between min and max
float getRandomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

// Particle structure
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f targetPosition;
    bool movingToTarget = false;
};

int main() {

    //std::cout << ENCODED_TEXT << "\n";

    // Initialize window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hehehe");
    window.setFramerateLimit(60);
    srand(static_cast<unsigned>(time(0)));

    // Load font
    sf::Font font;
    if (!font.loadFromFile("ariali.ttf")) {
        return -1; // Make sure you have a font file in the working directory
    }

    // Create target text object
    sf::Text targetText;
    targetText.setFont(font);
    targetText.setString(TARGET_TEXT);
    targetText.setCharacterSize(FONT_SIZE);
    //targetText.setFillColor(sf::Color::Red);
    targetText.setFillColor(sf::Color(255, 215, 0)); // RGB equivalent of #ffd700
    // Center text in the window
    sf::FloatRect textRect = targetText.getLocalBounds();
    targetText.setOrigin(textRect.left + textRect.width / 2.0f,
                         textRect.top + textRect.height / 2.0f);
    targetText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f - 50);

    // Load heart image
    sf::Image heartImage;
    if (!heartImage.loadFromFile(HEART_IMAGE_FILE)) {
        return -1; // Ensure you have 'heart.png' in the working directory
    }

    // Create a sprite for the heart and position it below the text
    sf::Texture heartTexture;
    heartTexture.loadFromImage(heartImage);
    sf::Sprite heartSprite(heartTexture);
    heartSprite.setScale(0.1f, 0.1f); // Scale down the heart image if needed

    // Center the heart below the text
    heartSprite.setPosition(WINDOW_WIDTH / 2.0f - heartSprite.getGlobalBounds().width / 2,
                            WINDOW_HEIGHT / 2.0f + textRect.height / 2.0f);

    // Render text and heart to texture to get target positions for particles
    sf::RenderTexture renderTexture;
    renderTexture.create(WINDOW_WIDTH, WINDOW_HEIGHT);
    renderTexture.clear(sf::Color::Black);
    renderTexture.draw(targetText);
    renderTexture.draw(heartSprite);
    renderTexture.display();

    sf::Image textAndHeartImage = renderTexture.getTexture().copyToImage();

    // Create particles
    std::vector<Particle> particles(PARTICLE_COUNT);
    int targetParticleCount = 0; // Count of particles used to render the text and heart
    for (auto& particle : particles) {
        // Initialize particles randomly within the window
        particle.position = sf::Vector2f(getRandomFloat(0, WINDOW_WIDTH), getRandomFloat(0, WINDOW_HEIGHT));
        particle.velocity = sf::Vector2f(getRandomFloat(-PARTICLE_SPEED, PARTICLE_SPEED),
                                         getRandomFloat(-PARTICLE_SPEED, PARTICLE_SPEED));
        particle.movingToTarget = false;
    }

    sf::Clock clock;
    float elapsed = 0.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update particles
        elapsed += clock.restart().asSeconds();
        for (auto& particle : particles) {
            if (elapsed >= TEXT_CONVERGE_TIME && !particle.movingToTarget && targetParticleCount < PARTICLE_COUNT / 3) {
                // Determine target position based on the rendered text and heart image
                int x = static_cast<int>(particle.position.x);
                int y = static_cast<int>(particle.position.y);

                if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT) {
                    // If the pixel in the target text-and-heart image is not black, it's part of the target
                    if (textAndHeartImage.getPixel(x, y).r > 0) {
                        particle.targetPosition = sf::Vector2f(static_cast<float>(x), static_cast<float>(y));
                        particle.movingToTarget = true;
                        targetParticleCount++;
                    }
                }
            }

            if (!particle.movingToTarget) {
                // Random vibration
                particle.position += particle.velocity * clock.getElapsedTime().asSeconds();
                // Keep particles inside the window bounds
                if (particle.position.x < 0 || particle.position.x > WINDOW_WIDTH)
                    particle.velocity.x = -particle.velocity.x;
                if (particle.position.y < 0 || particle.position.y > WINDOW_HEIGHT)
                    particle.velocity.y = -particle.velocity.y;
            } else {
                // Move particle towards target position
                sf::Vector2f direction = particle.targetPosition - particle.position;
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (distance > 0.5f) {
                    direction /= distance; // Normalize
                    particle.position += direction * PARTICLE_SPEED * clock.getElapsedTime().asSeconds();
                }
            }
        }

        // Draw particles
        window.clear(sf::Color(26, 26, 64));
        for (const auto& particle : particles) {
            sf::CircleShape shape(2.0f);
            shape.setPosition(particle.position);
            shape.setFillColor(particle.movingToTarget ? sf::Color::Red : sf::Color::Blue);
            window.draw(shape);
        }
        window.display();
    }

    return 0;
}
