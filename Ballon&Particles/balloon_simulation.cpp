#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int NUM_PARTICLES = 800;
const float PARTICLE_RADIUS = 5.0f;
const float BALLOON_RADIUS = 100.0f;
const float PARTICLE_SPEED = 100.0f;
const int BALLOON_SEGMENTS = 100;
const float BALLOON_SPEED = 50.0f;

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

// Helper function to create a circular balloon shape
void createBalloon(std::vector<sf::Vector2f>& balloonVertices, const sf::Vector2f& balloonCenter) {
    balloonVertices.clear();
    for (int i = 0; i < BALLOON_SEGMENTS; ++i) {
        float angle = 2 * M_PI * i / BALLOON_SEGMENTS;
        sf::Vector2f point = balloonCenter + sf::Vector2f(std::cos(angle), std::sin(angle)) * BALLOON_RADIUS;
        balloonVertices.push_back(point);
    }
}

void updateParticlePosition(Particle& particle, float dt) {
    particle.shape.move(particle.velocity * dt);
}

void checkCollisionWithBalloon(Particle& particle, const sf::Vector2f& balloonCenter) {
    sf::Vector2f position = particle.shape.getPosition();
    sf::Vector2f centerToParticle = position - balloonCenter;
    float distance = std::sqrt(centerToParticle.x * centerToParticle.x + centerToParticle.y * centerToParticle.y);

    if (distance + PARTICLE_RADIUS > BALLOON_RADIUS) {
        // Reflect velocity elastically
        sf::Vector2f normal = centerToParticle / distance;
        particle.velocity -= 2.0f * (particle.velocity.x * normal.x + particle.velocity.y * normal.y) * normal;

        // Adjust position to prevent sticking
        float overlap = (distance + PARTICLE_RADIUS) - BALLOON_RADIUS;
        particle.shape.move(-normal * overlap);
    }
}

void updateBalloonShape(const std::vector<Particle>& particles, std::vector<sf::Vector2f>& balloonVertices, const sf::Vector2f& balloonCenter) {
    // Reset balloon shape to initial circle
    createBalloon(balloonVertices, balloonCenter);

    // Deform balloon based on nearby particles
    for (const auto& particle : particles) {
        sf::Vector2f position = particle.shape.getPosition();
        for (auto& vertex : balloonVertices) {
            sf::Vector2f vertexToParticle = position - vertex;
            float distance = std::sqrt(vertexToParticle.x * vertexToParticle.x + vertexToParticle.y * vertexToParticle.y);
            if (distance < PARTICLE_RADIUS * 3) {
                vertex += vertexToParticle / distance * 5.0f; // Temporary deformation
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Floating Balloon with Particles");
    window.setFramerateLimit(60);

    // Initialize random seed
    std::srand(std::time(nullptr));

    // Create particles
    std::vector<Particle> particles(NUM_PARTICLES);
    for (auto& particle : particles) {
        particle.shape.setRadius(PARTICLE_RADIUS);
        particle.shape.setFillColor(sf::Color::White);
        float angle = std::rand() % 360 * M_PI / 180.0f;
        particle.velocity = sf::Vector2f(std::cos(angle), std::sin(angle)) * PARTICLE_SPEED;
    }

    // Initialize balloon center and motion
    sf::Vector2f balloonCenter(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    sf::Vector2f balloonVelocity(
        (std::rand() % 2 == 0 ? 1 : -1) * BALLOON_SPEED,
        (std::rand() % 2 == 0 ? 1 : -1) * BALLOON_SPEED);

    // Create balloon
    std::vector<sf::Vector2f> balloonVertices;
    createBalloon(balloonVertices, balloonCenter);

    // Randomize particle positions inside the balloon
    for (auto& particle : particles) {
        float angle = std::rand() * 2 * M_PI / RAND_MAX;
        float radius = std::sqrt(std::rand() / float(RAND_MAX)) * BALLOON_RADIUS; // Random radius within the balloon
        sf::Vector2f position = balloonCenter + sf::Vector2f(std::cos(angle), std::sin(angle)) * radius;
        particle.shape.setPosition(position);
    }

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        float dt = clock.restart().asSeconds();

        // Update balloon position
        balloonCenter += balloonVelocity * dt;

        // Bounce balloon off the window edges
        if (balloonCenter.x - BALLOON_RADIUS < 0 || balloonCenter.x + BALLOON_RADIUS > WINDOW_WIDTH) {
            balloonVelocity.x = -balloonVelocity.x;
        }
        if (balloonCenter.y - BALLOON_RADIUS < 0 || balloonCenter.y + BALLOON_RADIUS > WINDOW_HEIGHT) {
            balloonVelocity.y = -balloonVelocity.y;
        }

        // Update particles
        for (auto& particle : particles) {
            updateParticlePosition(particle, dt);
            checkCollisionWithBalloon(particle, balloonCenter);
        }

        // Update balloon shape
        updateBalloonShape(particles, balloonVertices, balloonCenter);

        // Render
        window.clear(sf::Color::Black);

        // Draw balloon
        sf::VertexArray balloon(sf::LineStrip, BALLOON_SEGMENTS + 1);
        for (size_t i = 0; i < balloonVertices.size(); ++i) {
            balloon[i].position = balloonVertices[i];
            balloon[i].color = sf::Color::Red;
        }
        balloon[BALLOON_SEGMENTS] = balloon[0]; // Close the loop
        window.draw(balloon);

        // Draw particles
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }

        window.display();
    }

    return 0;
}
