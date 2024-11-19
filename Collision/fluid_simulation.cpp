#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>

const int WINDOW_SIZE = 500;
const int NUM_PARTICLES = 700;
const float PARTICLE_RADIUS = 0.1f;
const float VELOCITY_SCALE = 10.0f;
const float REPULSION_FORCE_AMPLITUDE = 500.0f; // Amplitude of the Gaussian function
const float SIGMA = 20.0f; // Controls the width of the Gaussian function
const float ATTRACTOR_STRENGTH = 5000.0f; // Strength of the attractor force
const float MIN_DISTANCE = 500.0f; // Minimum distance at which the attractor starts influencing
const float DENSITY_RADIUS = 30.0f; // Radius to consider for density calculation

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float density; // New density attribute

    Particle(float x, float y, float vx, float vy)
        : position(x, y), velocity(vx, vy), density(0.0f) {}
};

float gaussianRepulsion(float distance) {
    return REPULSION_FORCE_AMPLITUDE * std::exp(-distance * distance / (2 * SIGMA * SIGMA));
}

void updateParticle(Particle& particle, const std::vector<Particle>& particles, float dt, const sf::Vector2f& attractorPosition, bool isAttracting) {
    // Initialize repulsion force to zero
    sf::Vector2f repulsionForce(0.0f, 0.0f);

    // Reset the particle's density
    particle.density = 0.0f;

    // Calculate the repulsion force from all other particles and compute density
    for (const auto& other : particles) {
        if (&particle != &other) { // Don't apply force from itself
            sf::Vector2f diff = particle.position - other.position;
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (distance > 0.1f) { // Avoid division by zero
                float forceMagnitude = gaussianRepulsion(distance);
                repulsionForce += forceMagnitude * (diff / distance);

                // Calculate density based on proximity
                if (distance < DENSITY_RADIUS) {
                    particle.density += 1.0f; // Increment density for nearby particles
                }
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

sf::Color densityToColor(float density) {
    // Map the density to a color between blue (low) and red (high)
    int red = std::min(255, static_cast<int>(density * 255.0f / 15.0f));
    int blue = 255 - red;
    return sf::Color(red, 0, blue, 255); // Ensure alpha is 255
}

void drawGlowingParticle(sf::RenderWindow& window, const sf::Vector2f& position, sf::Color color) {
    // Draw multiple layers of circles to create a glow effect
    for (int i = 0; i < 10; ++i) {
        sf::CircleShape shape(PARTICLE_RADIUS + i * .5f);
        shape.setOrigin(shape.getRadius(), shape.getRadius());
        shape.setPosition(position);
        sf::Color layerColor = color;
        layerColor.a = static_cast<sf::Uint8>(255 - i * 50); // Gradually decrease alpha
        shape.setFillColor(layerColor);
        window.draw(shape);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Particle Fluid Simulation with Attractor and Glow Effect");
   
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

        window.clear(sf::Color(10, 0, 192, 255));

        // Draw particles with a glow effect
        for (const auto& particle : particles) {
            sf::Color color = densityToColor(particle.density); // Set the color based on density
            drawGlowingParticle(window, particle.position, color);
        }

        window.display();
    }

    return 0;
}
