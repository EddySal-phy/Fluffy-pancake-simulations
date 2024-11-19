#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

const int WINDOW_SIZE = 600;
const int NUM_PARTICLES = 600;
const float PARTICLE_RADIUS = 10.0f;
const float VELOCITY_SCALE = 1000.0f;
const float REPULSION_FORCE_AMPLITUDE = 100.0f; // Amplitude of the Gaussian function
const float SIGMA = 50.0f; // Controls the width of the Gaussian function
const float ATTRACTOR_STRENGTH = 5000.0f; // Strength of the attractor force
const float MIN_DISTANCE = 500.0f; // Minimum distance at which the attractor starts influencing

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;

    Particle(float x, float y, float vx, float vy)
        : position(x, y), velocity(vx, vy) {}
};

float gaussianRepulsion(float distance) {
    return REPULSION_FORCE_AMPLITUDE * std::exp(-distance * distance / (2 * SIGMA * SIGMA));
}

void updateParticle(Particle& particle, const std::vector<Particle>& particles, float dt, const sf::Vector2f& attractorPosition, bool isAttracting) {
    // Initialize repulsion force to zero
    sf::Vector2f repulsionForce(0.0f, 0.0f);

    // Calculate the repulsion force from all other particles
    for (const auto& other : particles) {
        if (&particle != &other) { // Don't apply force from itself
            sf::Vector2f diff = particle.position - other.position;
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (distance > 0.1f) { // Avoid division by zero
                float forceMagnitude = gaussianRepulsion(distance);
                repulsionForce += forceMagnitude * (diff / distance);
            }
        }
    }

    // Apply the repulsion force to the particle's velocity
    particle.velocity += repulsionForce * dt;

    // Apply the attractor force if the particle is within the influence range and attraction is active
    if (isAttracting) {
        sf::Vector2f attractorDiff = attractorPosition - particle.position;
        float attractorDistance = std::sqrt(attractorDiff.x * attractorDiff.x + attractorDiff.y * attractorDiff.y);
        if (attractorDistance < MIN_DISTANCE) {
            float attractorForceMagnitude = ATTRACTOR_STRENGTH / (attractorDistance + 0.1f); // Avoid division by zero
            particle.velocity += attractorForceMagnitude * (attractorDiff / attractorDistance) * dt;
        }
    }

    // Update the particle's position
    particle.position += particle.velocity * dt;

    // Bounce off walls
    if (particle.position.x < PARTICLE_RADIUS) {
        particle.position.x = PARTICLE_RADIUS;
        particle.velocity.x *= -1;
    }
    if (particle.position.x > WINDOW_SIZE - PARTICLE_RADIUS) {
        particle.position.x = WINDOW_SIZE - PARTICLE_RADIUS;
        particle.velocity.x *= -1;
    }
    if (particle.position.y < PARTICLE_RADIUS) {
        particle.position.y = PARTICLE_RADIUS;
        particle.velocity.y *= -1;
    }
    if (particle.position.y > WINDOW_SIZE - PARTICLE_RADIUS) {
        particle.position.y = WINDOW_SIZE - PARTICLE_RADIUS;
        particle.velocity.y *= -1;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Particle Fluid Simulation with Attractor");

    // Create particles
    std::vector<Particle> particles;
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        float x = static_cast<float>(rand() % WINDOW_SIZE);
        float y = static_cast<float>(rand() % WINDOW_SIZE);
        float vx = static_cast<float>((rand() % 100 - 50) / VELOCITY_SCALE);
        float vy = static_cast<float>((rand() % 100 - 50) / VELOCITY_SCALE);
        particles.emplace_back(x, y, vx, vy);
    }

    sf::Clock clock;
    sf::Vector2f attractorPosition(-1.0f, -1.0f); // Initial invalid position
    bool isAttracting = false; // Track whether the mouse is pressed

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                attractorPosition = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                isAttracting = true; // Start attracting particles
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                isAttracting = false; // Stop attracting particles
            }
        }

        float dt = clock.restart().asSeconds();

        // Update particles with repulsion and attractor forces
        for (auto& particle : particles) {
            updateParticle(particle, particles, dt, attractorPosition, isAttracting);
        }

        window.clear();

        // Draw particles
        for (const auto& particle : particles) {
            sf::CircleShape shape(PARTICLE_RADIUS);
            shape.setPosition(particle.position - sf::Vector2f(PARTICLE_RADIUS, PARTICLE_RADIUS));
            shape.setFillColor(sf::Color::White);
            window.draw(shape);
        }

        window.display();
    }

    return 0;
}
