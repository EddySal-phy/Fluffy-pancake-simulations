#include <SFML/Graphics.hpp>
#include <cmath>
#include "config.hpp"
#include "pendulum.hpp"
#include "ode_solver.hpp"


void phys_obj::pendulum::draw(sf::RenderWindow &window){
        sf::VertexArray vertices(sf::Lines, 2);
        vertices[0] = sf::Vertex(sf::Vector2f(x_nail, y_nail)); 
        vertices[1] = sf::Vertex(sf::Vector2f(x_ball, y_ball));

        window.draw(vertices);
}
void phys_obj::update(phys_obj::pendulum &pendulum_I, phys_obj::pendulum &pendulum_II, phys_obj::state &new_dp_state){

    pendulum_I.theta = new_dp_state.theta.first;
    pendulum_I.theta_vel = new_dp_state.omega.first;
    pendulum_I.x_ball = pendulum_I.length * sin(pendulum_I.theta) + pendulum_I.x_nail;
    pendulum_I.y_ball = pendulum_I.length * cos(pendulum_I.theta) + pendulum_I.y_nail;
    
    pendulum_II.theta_vel = new_dp_state.omega.second;
    pendulum_II.theta = new_dp_state.theta.second;
    pendulum_II.x_nail = pendulum_I.x_ball;
    pendulum_II.y_nail = pendulum_I.y_ball;
    pendulum_II.x_ball = pendulum_II.length * sin(pendulum_II.theta) + pendulum_II.x_nail;
    pendulum_II.y_ball = pendulum_II.length * cos(pendulum_II.theta) + pendulum_II.y_nail;
}
