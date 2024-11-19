#include "ode_sol.hpp"
#include <cmath>

std::pair<double, double> ode_sol::rk4_2nd_ODE(double y_0, double dydt_0, double dt){
        
    /*

     4th order Runge-Kutta method for solving 2nd order ODE;
            
    Explanation:

            Solve 

            d^2y/dx^2 + dy/dx + f(x, y) = 0
    
            We parametrize as
                z := dy/dt 
                d^2y /dt^2 := dz/dt

            returns a pair of values for next step.

            See, as example, https://math.stackexchange.com/questions/721076/help-with-using-the-runge-kutta-4th-order-method-on-a-system-of-2-first-order-od
    
    Parameters:
            y_0: initial value of y
            dydt_0: initial value of dydt
            dt: stepsize
    */


    double &z0 = dydt_0;
    
    double k0 = z0 * dt;    
    double l0 = dt * -1* (conf::g / conf::length_1) * sin(y_0);

    double k1 = dt * (z0  + 1/2 * l0);
    double l1 = dt * -1* (conf::g / conf::length_1) * sin(y_0 + 1/2 * l0);

    double k2 = dt * (z0  + 1/2 * l1);
    double l2 = dt * -1* (conf::g / conf::length_1) * sin(y_0 + 1/2 * k1);

    double k3 =  dt * (z0  + l2);
    double l3 = dt * -1* (conf::g / conf::length_1) * sin(y_0 + k2);

    return {y_0 + (k0 + 2*k1 + 2*k2 + k3)/6, z0 + (l0 + 2*l1 + 2*l2 + l3)/6}; 




}