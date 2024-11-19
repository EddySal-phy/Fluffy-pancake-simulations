#include "wind_rend.hpp"
#include <iostream>
#include <vector>
#include "pendulum.hpp"
void win::processEvents(sf::RenderWindow &window){
    sf::Event event;
    while (window.pollEvent(event)){
        if (event.type == sf::Event::Closed){
            window.close();
        }
        if (event.type == sf::Event::KeyPressed){
            if (event.key.code == sf::Keyboard::Escape){
                window.close();
            }
        }
    }
}

void win::textWindow(sf::RenderWindow &window, std::string str_text, int size){
    
    sf::Font font;
    sf::Text text;

    if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/Arial_Italic.ttf")) {
        std::cerr << "Error loading font" << std::endl;
    }

    text.setFont(font);
    text.setString(str_text);
    text.setCharacterSize(size);
    text.setStyle(sf::Text::Bold);
    text.setPosition(sf::Vector2f(10, 10)); 
    window.draw(text);
}
