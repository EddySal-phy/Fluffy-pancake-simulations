#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include "win_rend.hpp"
#include "config.hpp"
#include "system.hpp"
#include "ode_sol.hpp"



int main(){

    sf::RenderWindow window (sf::VideoMode(conf::window_height, conf::window_width), "Simple Pendulum");
    window.setFramerateLimit(conf::frame_rate);

    syst::state pendulum_1(conf::mass_1, conf::length_1, conf::theta_1, conf::theta_vel_1);

    while (window.isOpen())
    {   
        // Process events
        win::processEvents(window); 
        
        // Render
        pendulum_1.update(pendulum_1, 0.2, 0.01);
        window.clear();
        pendulum_1.draw(window);

        // Text
        std::stringstream ss;
        ss << "Energy: " << pendulum_1.get_energy(pendulum_1) << std::endl;
        win::textWindow(window, ss.str(), conf::text_size);

        window.display();



    }


  
}