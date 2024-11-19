#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <sstream>
// Union-Find (Disjoint-Set) Structure to manage clusters
class UnionFind {
public:
    std::vector<int> parent, size;
    int n;

    UnionFind(int n) : n(n), parent(n), size(n, 1) {
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }

    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            if (size[rootX] < size[rootY]) std::swap(rootX, rootY);
            parent[rootY] = rootX;
            size[rootX] += size[rootY];
        }
    }
};

// Ordered color palette
std::vector<sf::Color> createColorPalette() {
    std::vector<sf::Color> palette = {
        sf::Color::Blue, sf::Color::Green, sf::Color::Red, sf::Color::Yellow,
        sf::Color::Cyan, sf::Color::Magenta, sf::Color(255, 165, 0),  // Orange
        sf::Color(128, 0, 128),  // Purple
        sf::Color(255, 192, 203),  // Pink
        sf::Color(64, 224, 208),  // Turquoise
        sf::Color(255, 215, 0),  // Gold
        sf::Color(139, 69, 19),  // Brown
        sf::Color(75, 0, 130)   // Indigo
    };
    return palette;
}

// Function to handle slider movement
float handleSlider(sf::RectangleShape& sliderBar, sf::CircleShape& sliderHandle, sf::RenderWindow& window, float currentP) {
    // Get mouse position
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // Check if the mouse button is pressed and the mouse is inside the slider area
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
        mousePos.x >= sliderBar.getPosition().x &&
        mousePos.x <= sliderBar.getPosition().x + sliderBar.getSize().x &&
        mousePos.y >= sliderBar.getPosition().y &&
        mousePos.y <= sliderBar.getPosition().y + sliderBar.getSize().y) {

        // Move the handle based on mouse X position, clamp it to the slider's bounds
        float newPosX = std::max(sliderBar.getPosition().x, std::min((float)mousePos.x, sliderBar.getPosition().x + sliderBar.getSize().x));
        sliderHandle.setPosition(newPosX - sliderHandle.getRadius(), sliderHandle.getPosition().y);

        // Calculate new probability 'p' based on handle position
        float p = (newPosX - sliderBar.getPosition().x) / sliderBar.getSize().x;
        return p;
    }

    return currentP;  // No change in p if slider wasn't moved
}

// Function to handle arrow key movements
float handleKeyInput(float p, sf::RectangleShape& sliderBar, sf::CircleShape& sliderHandle) {
    float step = 0.001f;  // How much p should change with each key press

    // Handle left and right arrow keys to adjust the value of p
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        p = std::min(1.0f, p + step);  // Increment p and clamp to 1.0
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        p = std::max(0.0f, p - step);  // Decrement p and clamp to 0.0
    }

    // Update the slider handle position to reflect the change in p
    float newPosX = sliderBar.getPosition().x + p * sliderBar.getSize().x;
    sliderHandle.setPosition(newPosX - sliderHandle.getRadius(), sliderHandle.getPosition().y);

    return p;
}

void updateGrid(float p, int gridSize, std::vector<bool>& occupied, std::vector<sf::Color>& clusterColors, UnionFind& uf, const std::vector<sf::Color>& colorPalette) {
    // Reset union-find and grid
    uf = UnionFind(gridSize * gridSize);  // Reset Union-Find for new clusters

    // Reset grid and assign colors from the palette
    int colorIndex = 0;
    for (int i = 0; i < gridSize * gridSize; i++) {
        occupied[i] = ((rand() / (float)RAND_MAX) < p);
        if (occupied[i]) {
            clusterColors[i] = colorPalette[colorIndex % colorPalette.size()];
            colorIndex++;
        } else {
            clusterColors[i] = sf::Color::White;
        }
    }

    // Apply percolation rule: Unite adjacent occupied sites
    for (int y = 0; y < gridSize; y++) {
        for (int x = 0; x < gridSize; x++) {
            int index = y * gridSize + x;
            if (!occupied[index]) continue;

            // Check right neighbor
            if (x + 1 < gridSize && occupied[y * gridSize + (x + 1)])
                uf.unite(index, y * gridSize + (x + 1));

            // Check bottom neighbor
            if (y + 1 < gridSize && occupied[(y + 1) * gridSize + x])
                uf.unite(index, (y + 1) * gridSize + x);
        }
    }
}

int main() {
    // Grid size
    const int gridSize = 50;  // Number of cells along one axis
    const int cellSize = 10;  // Pixel size of each cell
    const int windowSize = gridSize * cellSize;
    const int sliderHeight = 40;  // Height of slider area
    float p = 1.0;  // Initial probability of site occupation (p=1)

    // Initialize random seed
    srand(static_cast<unsigned>(time(0)));

    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(windowSize, windowSize + sliderHeight), "Percolation Simulation with Ordered Color Palette");
    window.setFramerateLimit(60);

    // Slider properties
    sf::RectangleShape sliderBar(sf::Vector2f(windowSize * 0.8f, 10));  // Slider bar
    sliderBar.setPosition(windowSize * 0.1f, windowSize + 15);
    sliderBar.setFillColor(sf::Color(200, 200, 200));

    sf::CircleShape sliderHandle(10);  // Slider handle (circle)
    sliderHandle.setFillColor(sf::Color::Red);
    sliderHandle.setOrigin(sliderHandle.getRadius(), sliderHandle.getRadius());
    sliderHandle.setPosition(sliderBar.getPosition().x + p * sliderBar.getSize().x, sliderBar.getPosition().y + sliderBar.getSize().y / 2);

    // Grid and cluster management
    UnionFind uf(gridSize * gridSize);  // Union-Find to track clusters
    std::vector<bool> occupied(gridSize * gridSize, false);  // Grid state
    std::vector<sf::Color> clusterColors(gridSize * gridSize, sf::Color::White);  // Colors for each cluster

    // Ordered color palette
    std::vector<sf::Color> colorPalette = createColorPalette();

    // Initial grid generation
    updateGrid(p, gridSize, occupied, clusterColors, uf, colorPalette);

    while (window.isOpen()) {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }

        // Update p based on slider position or key press
        float newP = handleSlider(sliderBar, sliderHandle, window, p);
        newP = handleKeyInput(newP, sliderBar, sliderHandle);

        // If p changes, update the grid
        if (newP != p) {
            p = newP;
            updateGrid(p, gridSize, occupied, clusterColors, uf, colorPalette);  // Update the grid only when `p` changes
        }

        // Draw the grid and slider
        window.clear();

        // Draw the grid
        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                int index = y * gridSize + x;

                sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
                cell.setPosition(x * cellSize, y * cellSize);

                if (occupied[index]) {
                    // Assign color based on cluster root
                    int root = uf.find(index);
                    cell.setFillColor(clusterColors[root]);
                } else {
                    cell.setFillColor(sf::Color::White);
                }

                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color::Black);
                window.draw(cell);
            }
        }

        //Text
            sf::Font font;
        if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/ariali.ttf")) {
            return -1;
        }

        // Create the text
        sf::Text p_Text;
        p_Text.setFont(font);
        p_Text.setCharacterSize(24);
        p_Text.setFillColor(sf::Color::Black);
        p_Text.setPosition(10.f, 10.f);

        // Draw the slider
        window.draw(sliderBar);
        window.draw(sliderHandle);

        // Draw p value of slider.
        std::stringstream ss;
        ss << "p = " << (round(p*100))/100 << std::endl;
        p_Text.setString(ss.str());
        window.draw(p_Text);
        window.display();
    }

    return 0;
}
