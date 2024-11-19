#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <SFML/Graphics.hpp>


namespace conf
{   

    // Windows configuration
    const int window_height = 600;
    const int window_width = 600;
    const int orgin_x = window_height/2;
    const int orgin_y = window_width/5;
    const float frame_rate = 60;
    const int text_size = 24;

    // Constants
    const double PI = 3.14159265358979323846;
    const double g = 9.8;
    
    
    // Particle 1 initial conditions
    const float mass_1 = 1; // kg
    const float length_1 = 100; // m
    const float theta_1 = conf::PI/6; // rad 
    const float theta_vel_1 = 0; // rad/s
} 


#endif