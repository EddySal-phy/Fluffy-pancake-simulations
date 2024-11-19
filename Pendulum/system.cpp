#include <SFML/Graphics.hpp>
#include "config.hpp"
#include "system.hpp"
#include "ode_sol.hpp"
#include <cmath>

void syst::state::draw(sf::RenderWindow &window){
        sf::VertexArray vertices(sf::Lines, 2);
        vertices[0] = sf::Vertex(sf::Vector2f(conf::orgin_x, conf::orgin_y)); 
        vertices[1] = sf::Vertex(sf::Vector2f(x, y));

        window.draw(vertices);
}

syst::state syst::state::update(syst::state& st, double time, double dt){
    
        double time_passed = 0.0;
        
        
        double  st_theta = st.theta;
        double  st_theta_vel = st.theta_vel;

        while(time_passed < time)
        {
            std::pair<double, double> sol_next =  ode_sol::rk4_2nd_ODE(st_theta, st_theta_vel, dt);
            st_theta = sol_next.first;
            st_theta_vel = sol_next.second;
            time_passed += dt;
        }

        this->theta = st_theta;
        this->theta_vel = st_theta_vel;
        this->x = st.length * sin(st.theta) + conf::orgin_x;
        this->y = st.length * cos(st.theta) + conf::orgin_y;
        
        return st;

}   

double syst::state::get_theta(){
    return theta;
}

double syst::state::get_theta_vel(){
    return theta_vel;
}

double syst::state::get_x(){    
    return x;
}

double syst::state::get_y(){
    return y;
}   

double syst::state::get_length(){
    return length;
}
double syst::state::get_energy(syst::state& st){

    // Potential energy is defined with the ceil as V=0; Potential energy is assumed to be negative.
    return 0.5 * st.mass * st.length * st.length * st.theta_vel * st.theta_vel + st.mass * conf::g  * st.length * abs(1- cos(st.theta));
}