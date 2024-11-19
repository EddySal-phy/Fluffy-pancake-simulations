#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <unordered_set>

// Constants
const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 200;
const int NUM_BIRDS = 100;
const float MAX_SPEED = 50.0f; // pixels per second
const float FOLLOW_RADIUS = 20.0f; // pixels
const float PROBABILITY_SCALE = 1.0f; // Scale for probability function
const float TIME_STEP = 0.1f; // seconds per update

// Structure to represent a Bird
struct Bird {
    sf::Vector2f position;
    sf::Vector2f velocity;
    int following; // Index of the bird it follows, -1 if none

    Bird(sf::Vector2f pos, sf::Vector2f vel)
        : position(pos), velocity(vel), following(-1) {}
};

// Function to generate a random float between min and max
float randomFloat(float min, float max) {
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    return min + r * (max - min);
}

// Function to apply periodic boundary conditions
void applyPeriodicBoundary(sf::Vector2f& pos) {
    if (pos.x < 0) pos.x += WINDOW_WIDTH;
    if (pos.x >= WINDOW_WIDTH) pos.x -= WINDOW_WIDTH;
    if (pos.y < 0) pos.y += WINDOW_HEIGHT;
    if (pos.y >= WINDOW_HEIGHT) pos.y -= WINDOW_HEIGHT;
}

// Probability function based on distance
float followProbability(float distance) {
    // Example: Probability decreases with distance
    if (distance > FOLLOW_RADIUS) return 0.0f;
    return PROBABILITY_SCALE * (1.0f - distance / FOLLOW_RADIUS);
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    // Initialize birds
    std::vector<Bird> birds;
    birds.reserve(NUM_BIRDS);
    for (int i = 0; i < NUM_BIRDS; ++i) {
        float x = randomFloat(0, WINDOW_WIDTH);
        float y = randomFloat(0, WINDOW_HEIGHT);
        // Random velocity direction
        float angle = randomFloat(0, 2 * M_PI);
        float speed = randomFloat(10.0f, MAX_SPEED);
        sf::Vector2f vel = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        birds.emplace_back(sf::Vector2f(x, y), vel);
    }

    // SFML setup
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Bird Simulation");
    window.setFramerateLimit(60);

    // Bird visual representation
    sf::CircleShape birdShape(3.0f);
    birdShape.setFillColor(sf::Color::White);

    // Data file setup
    std::ofstream dataFile("groups_over_time.dat");
    if (!dataFile.is_open()) {
        std::cerr << "Failed to open data file for writing.\n";
        return -1;
    }

    float elapsedTime = 0.0f;

    // Main loop
    while (window.isOpen()) {
        // Event processing
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update birds
        for (int i = 0; i < NUM_BIRDS; ++i) {
            Bird& bird = birds[i];

            // If not following anyone, check for birds to follow
            if (bird.following == -1) {
                for (int j = 0; j < NUM_BIRDS; ++j) {
                    if (i == j) continue;
                    Bird& other = birds[j];

                    // Compute distance with periodic boundaries
                    float dx = std::abs(bird.position.x - other.position.x);
                    float dy = std::abs(bird.position.y - other.position.y);
                    dx = std::min(dx, WINDOW_WIDTH - dx);
                    dy = std::min(dy, WINDOW_HEIGHT - dy);
                    float distance = std::sqrt(dx * dx + dy * dy);

                    if (distance <= FOLLOW_RADIUS) {
                        float prob = followProbability(distance);
                        float randVal = randomFloat(0.0f, 1.0f);
                        if (randVal < prob) {
                            bird.following = j;
                            // Adjust velocity to follow the leader
                            sf::Vector2f direction = other.position - bird.position;
                            // Apply periodic boundary to direction
                            if (direction.x > WINDOW_WIDTH / 2) direction.x -= WINDOW_WIDTH;
                            if (direction.x < -WINDOW_WIDTH / 2) direction.x += WINDOW_WIDTH;
                            if (direction.y > WINDOW_HEIGHT / 2) direction.y -= WINDOW_HEIGHT;
                            if (direction.y < -WINDOW_HEIGHT / 2) direction.y += WINDOW_HEIGHT;
                            // Normalize and set to MAX_SPEED
                            float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                            if (len != 0) {
                                bird.velocity = (direction / len) * MAX_SPEED;
                            }
                            break; // Start following the first bird that meets the condition
                        }
                    }
                }
            } else {
                // Continue following the leader
                Bird& leader = birds[bird.following];
                sf::Vector2f direction = leader.position - bird.position;
                // Apply periodic boundary to direction
                if (direction.x > WINDOW_WIDTH / 2) direction.x -= WINDOW_WIDTH;
                if (direction.x < -WINDOW_WIDTH / 2) direction.x += WINDOW_WIDTH;
                if (direction.y > WINDOW_HEIGHT / 2) direction.y -= WINDOW_HEIGHT;
                if (direction.y < -WINDOW_HEIGHT / 2) direction.y += WINDOW_HEIGHT;
                // Normalize and set to MAX_SPEED
                float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                if (len != 0) {
                    bird.velocity = (direction / len) * MAX_SPEED;
                }
            }

            // Update position
            bird.position += bird.velocity * TIME_STEP;
            applyPeriodicBoundary(bird.position);
        }

        // Count groups (leaders)
        std::unordered_set<int> leaders;
        for (int i = 0; i < NUM_BIRDS; ++i) {
            if (birds[i].following == -1) {
                leaders.insert(i);
            } else {
                leaders.insert(birds[i].following);
            }
        }
        int numGroups = leaders.size();

        // Write to data file
        dataFile << elapsedTime << " " << numGroups << "\n";

        // Rendering
        window.clear(sf::Color::Black);
        for (const auto& bird : birds) {
            birdShape.setPosition(bird.position);
            window.draw(birdShape);
        }
        window.display();

        // Update time
        elapsedTime += TIME_STEP;

        // Simple timing to match TIME_STEP
        sf::sleep(sf::seconds(TIME_STEP));
    }

    dataFile.close();
    return 0;
}
