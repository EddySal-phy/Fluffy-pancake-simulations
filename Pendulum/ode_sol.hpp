#ifndef ODE_SOL_HPP
#define ODE_SOL_HPP
#include <cmath>
#include "config.hpp"

namespace ode_sol{

    // A specific solver of our 2nd order ODE's with Range-Kutta 4th order method.
    std::pair<double, double> rk4_2nd_ODE(double y_0, double dydt_0, double dt);
}

#endif


