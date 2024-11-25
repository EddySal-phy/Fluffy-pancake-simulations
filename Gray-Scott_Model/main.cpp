
/* A C++ program to solve and simulate the Gray-Scott model using SFML.
 * 
 * This is a reaction-diffusion model which can explain several patterns
 * found in nature, based on the dynamics of two fluids.
 *
 * The Gray-Scott model can be found in: 
 * https://groups.csail.mit.edu/mac/projects/amorphous/GrayScott/
*/


#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

// Parameters for the Gray-Scott model
const int WIDTH = 400;         // Width of the simulation grid
const int HEIGHT = 400;        // Height of the simulation grid
const float DU = 0.916f;        // Diffusion rate for U
const float DV = 0.18f;        // Diffusion rate for V
const float FEED = 0.095f;     // Feed rate of U
const float KILL = 0.06f;      // Kill rate of V
const float DELTA_T = 1.0f;    // Time step
const int ITERATIONS_PER_FRAME = 10; // Number of iterations per frame

// Initialize the grid with random blobs of U and V
void initializeGrid(std::vector<std::vector<float>>& u, std::vector<std::vector<float>>& v) {
  // Initialize the grid with default values
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      u[y][x] = 1.0f; // Start with U at maximum concentration
      v[y][x] = 0.0f; // V is initially 0
    }
  }

  // Seed multiple random blobs of U and V across the grid
  int numBlobs = 10; // Number of random blobs
  int blobSize = 10; // Size of each blob (radius)

  // Random number generator initialization
  std::srand(static_cast<unsigned>(std::time(0)));

  for (int i = 0; i < numBlobs; ++i) {
    // Generate random position for each blob
    int centerX = std::rand() % WIDTH;
    int centerY = std::rand() % HEIGHT;

    // Fill a circular region around the center with random values for U and V
    for (int y = -blobSize; y <= blobSize; ++y) {
      for (int x = -blobSize; x <= blobSize; ++x) {
        int posX = (centerX + x + WIDTH) % WIDTH;
        int posY = (centerY + y + HEIGHT) % HEIGHT;

        // Check if (x, y) is within the blob radius
        if (x * x + y * y <= blobSize * blobSize) {
          u[posY][posX] = 0.5f + static_cast<float>(std::rand()) / RAND_MAX * 0.5f; // Random variation in U
          v[posY][posX] = 0.25f + static_cast<float>(std::rand()) / RAND_MAX * 0.5f; // Random variation in V
        }
      }
    }
  }
}


//  Function to calculate the Laplacian of a grid point
float laplacian(const std::vector<std::vector<float>>& grid, int x, int y) {
  float result = 0.0f;
  result += grid[y][x] * -1.0f;
  result += grid[(y + 1) % HEIGHT][x] * 0.2f;
  result += grid[(y - 1 + HEIGHT) % HEIGHT][x] * 0.2f;
  result += grid[y][(x + 1) % WIDTH] * 0.2f;
  result += grid[y][(x - 1 + WIDTH) % WIDTH] * 0.2f;
  result += grid[(y + 1) % HEIGHT][(x + 1) % WIDTH] * 0.05f;
  result += grid[(y + 1) % HEIGHT][(x - 1 + WIDTH) % WIDTH] * 0.05f;
  result += grid[(y - 1 + HEIGHT) % HEIGHT][(x + 1) % WIDTH] * 0.05f;
  result += grid[(y - 1 + HEIGHT) % HEIGHT][(x - 1 + WIDTH) % WIDTH] * 0.05f;
  return result;
}

// Update grid with Gray-Scott model
void updateGrid(std::vector<std::vector<float>>& u, std::vector<std::vector<float>>& v) {
  std::vector<std::vector<float>> newU = u;
  std::vector<std::vector<float>> newV = v;

  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      float lapU = laplacian(u, x, y);
      float lapV = laplacian(v, x, y);

      // Gray-Scott's model equations
      float uvv = u[y][x] * v[y][x] * v[y][x];
      newU[y][x] = u[y][x] + (DU * lapU - uvv + FEED * (1.0f - u[y][x])) * DELTA_T;
      newV[y][x] = v[y][x] + (DV * lapV + uvv - (FEED + KILL) * v[y][x]) * DELTA_T;

      // Clamp values between 0 and 1
      newU[y][x] = std::min(1.0f, std::max(0.0f, newU[y][x]));
      newV[y][x] = std::min(1.0f, std::max(0.0f, newV[y][x]));
    }
  }

  u.swap(newU);
  v.swap(newV);
}

int main() {
  // Initialize SFML window
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Gray-Scott Model Simulation");
  window.setFramerateLimit(60);

  // Initialize the concentration grids
  std::vector<std::vector<float>> u(HEIGHT, std::vector<float>(WIDTH, 1.0f));
  std::vector<std::vector<float>> v(HEIGHT, std::vector<float>(WIDTH, 0.0f));
  initializeGrid(u, v);

  // Texture and sprite to visualize the grid
  sf::Texture texture;
  texture.create(WIDTH, HEIGHT);
  sf::Sprite sprite(texture);

  // Buffer for pixel data
  std::vector<sf::Uint8> pixels(WIDTH * HEIGHT * 4);

  // Main simulation loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    // Update the simulation multiple times per frame
    for (int i = 0; i < ITERATIONS_PER_FRAME; ++i) {
      updateGrid(u, v);
    }

    // Render the grid to the screen
    for (int y = 0; y < HEIGHT; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
        float color = u[y][x] - v[y][x];
        color = std::min(1.0f, std::max(0.0f, color));

        // Use color gradient based on U-V values
        pixels[4 * (y * WIDTH + x) + 0] = static_cast<sf::Uint8>(color * 255);  // Red channel
        pixels[4 * (y * WIDTH + x) + 1] = static_cast<sf::Uint8>((1.0f - color) * 255);  // Green channel
        pixels[4 * (y * WIDTH + x) + 2] = 128; // Blue channel (constant)
        pixels[4 * (y * WIDTH + x) + 3] = 255; // Alpha channel
      }
    }

    // Update texture and display it
    texture.update(pixels.data());
    window.clear();
    window.draw(sprite);
    window.display();
  }

  return 0;
}

