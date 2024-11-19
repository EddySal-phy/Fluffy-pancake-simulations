#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include "wind_rend.hpp"
#include "config.hpp"
#include "pendulum.hpp"
#include "ode_solver.hpp"

int main(){

    sf::RenderWindow window (sf::VideoMode(conf::window_height, conf::window_width), "Double Pendulum");
    window.setFramerateLimit(conf::frame_rate);

    // Double Pendulum initial conditions
    phys_obj::state  dp_state = {{conf::theta_1, conf::theta_2}, {conf::theta_vel_1, conf::theta_vel_2}};
    phys_obj::system dp_sys   = {{conf::mass_1, conf::mass_2}, {conf::length_1, conf::length_2}};

    // Pendulums initialization
    phys_obj::pendulum pendulum_I(dp_sys.mass.first, dp_sys.length.first,  dp_state.theta.first, dp_state.omega.first, conf::orgin_x, conf::orgin_y);  
    phys_obj::pendulum pendulum_II(dp_sys.mass.second, dp_sys.length.second,  dp_state.theta.second, dp_state.omega.second, pendulum_I.x_ball, pendulum_I.y_ball);

    // Initialize trails
    std::vector<sf::Vector2f> trail_II;

    while (window.isOpen())
    {   
        // Process events
        win::processEvents(window); 
        
        // Update double pendulum
        dp_state = dp::advance(dp_state, dp_sys, 0.2, 0.01);
        phys_obj::update(pendulum_I, pendulum_II, dp_state);
       
        // Update trails
        trail_II.push_back(sf::Vector2f(pendulum_II.x_ball, pendulum_II.y_ball));
        if (trail_II.size() > 1000) trail_II.erase(trail_II.begin()); // Limit the trail to prevent excessive memory usage
        sf::VertexArray trailVA_II(sf::LinesStrip, trail_II.size());
        for (size_t i = 0; i < trail_II.size(); i++){
            
        if (bool RAINBOW = true){
            trailVA_II[i].position = trail_II[i];
            int color_index = (i * 360 / trail_II.size()) % 360;
            sf::Uint8 r = 128 + (127 * std::sin(color_index * 3.14159 / 180.0));
            sf::Uint8 g = 128 + (127 * std::sin((color_index + 120) * 3.14159 / 180.0));
            sf::Uint8 b = 128 + (127 * std::sin((color_index + 240) * 3.14159 / 180.0));
            trailVA_II[i].color = sf::Color(r, g, b);
        }
        else{   trailVA_II[i].position = trail_II[i];
            trailVA_II[i].color = sf::Color::Red;
        }
        }
        
       
        // Update window
        window.clear();
        pendulum_I.draw(window);
        pendulum_II.draw(window);
        window.draw(trailVA_II);
        window.display();



        }


  
}
