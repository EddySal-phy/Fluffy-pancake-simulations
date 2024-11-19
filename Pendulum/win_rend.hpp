#ifndef WIN_REND_HPP
#define WIN_REND_HPP

#include <SFML/Graphics.hpp>




namespace win{
    
    void processEvents(sf::RenderWindow &window);
    void textWindow(sf::RenderWindow &window, std::string str_text, int size);

}


#endif