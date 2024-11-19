#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <SFML/Graphics.hpp>


namespace conf
{   

    // Windows configuration
    const int window_height = 600;
    const int window_width = 600;
    const int orgin_x = window_height/2;
    const int orgin_y = window_width/2;
    const float frame_rate = 60;
    const int text_size = 24;

    // Constants
    const double PI = 3.14159265358979323846;
    const double g = 9.8;
    
    
    // Pendulum_1 initial conditions
    const float mass_1 = 1; // kg
    const float length_1 = 100; // m
    const float theta_1 = conf::PI/6; // rad 
    const float theta_vel_1 = 0; // rad/s


    // Pendulum_2 initial conditions
    const float mass_2 = 1; // kg
    const float length_2 = 100; // m
    const float theta_2 = conf::PI/3; // rad 
    const float theta_vel_2 = 1; // rad/s
} 


#endif