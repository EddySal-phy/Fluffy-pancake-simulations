#ifndef     SYSTEM_HPP
#define     SYSTEM_HPP
#include <SFML/Graphics.hpp>
#include <cmath>
#include "config.hpp"

namespace syst{
    
    
    struct state
    {
        const double mass;
        const double length;
        double theta;
        double theta_vel;

        // Radial-Cart. coordinates
        double x = length * sin(theta) + conf::orgin_x; ;
        double y = length * cos(theta) + conf::orgin_y;;  

        // Constructor
        state(double mass, double length, double theta, double theta_vel) : mass(mass), length(length), theta(theta), theta_vel(theta_vel) {};

        void draw(sf::RenderWindow &window);
        state update(state& st, double time, double dt);
        
        double get_energy(state& st);
        double get_theta();
        double get_theta_vel();
        double get_x();
        double get_y();
        double get_length();




    };
     
}


#endif