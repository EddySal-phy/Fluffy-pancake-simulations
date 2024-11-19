#ifndef     PENDULUM_HPP
#define     PENDULUM_HPP
#include <SFML/Graphics.hpp>
#include <cmath>
#include "config.hpp"

namespace phys_obj{
    
    struct state {
        std::pair<double, double> theta;
        std::pair<double, double> omega;
    };

    struct system {
        std::pair<double, double> mass;
        std::pair<double, double> length;
    };

    struct pendulum
    {
        const double mass;
        const double length;
        double theta;
        double theta_vel;
        double x_nail;
        double y_nail;
        
        // Radial-Cartesian. coordinates
        double x_ball = length * sin(theta) + x_nail;
        double y_ball = length * cos(theta) + y_nail;;  

        // Constructor
        pendulum(double mass, double length, double theta, double theta_vel, double x_nail, double y_nail) : mass(mass), length(length), theta(theta), 
                                                                                                            theta_vel(theta_vel), x_nail(x_nail), y_nail(y_nail) {};

        void draw(sf::RenderWindow &window);
    };
    
    void update(phys_obj::pendulum &pendulum_I, phys_obj::pendulum &pendulum_II, phys_obj::state &new_dp_state);
     
}


#endif