
#include <utility>
#include <cmath>
#include "config.hpp"
#include "pendulum.hpp"
#include "ode_solver.hpp"

namespace dp {

    double g  = conf::g;
    std::pair<double, double> operator+(const std::pair<double, double>& p1, const std::pair<double, double>& p2) noexcept {
        return {
            p1.first  + p2.first,
            p1.second + p2.second
        };
    }

    std::pair<double, double> operator*(const double d, const std::pair<double, double>& p) noexcept {
        return {
            d * p.first,
            d * p.second
        };
    }

    phys_obj::state operator+(const phys_obj::state& s1, const phys_obj::state& s2) noexcept {
        return {
            s1.theta + s2.theta,
            s1.omega + s2.omega
        };
    }

    phys_obj::state operator*(const double d, const phys_obj::state& s) noexcept {
        return {
            d * s.theta,
            d * s.omega
        };
    }



    phys_obj::state derive(const phys_obj::state& st, const phys_obj::system& ss) noexcept {

    /**
     * @brief Derivative function for double pendulum.
     * @param st current state of double pendulum
     * @param ss system parameters
     * @return derivative of the state
     * 
     * @note The derived equations are simplified and the original equations can be found in https://www.myphysicslab.com/dbl_pendulum/double-pendulum-en.html
     */

    const double delta = st.theta.second - st.theta.first;
    const double mass  = ss.mass.first + ss.mass.second;

    double s = sin(delta);
    double c = cos(delta);

    double denominator = mass * ss.length.first - ss.mass.second * ss.length.first * c * c;

    phys_obj::state derivative{{st.omega.first, st.omega.second}, {0, 0}};

    derivative.omega.first 
        = ss.mass.second * ss.length.first * st.omega.first * st.omega.first * s * c
        + ss.mass.second * g * sin(st.theta.second) * c
        + ss.mass.second * ss.length.second * st.omega.second * st.omega.second * s
        - mass * g * sin(st.theta.first);

    derivative.omega.first /= denominator;

    denominator *= ss.length.second / ss.length.first;           

    derivative.omega.second
        = - ss.mass.second * ss.length.second * st.omega.second
            * st.omega.second * s * c
        + mass * g * sin(st.theta.first) * c
        - mass * ss.length.first * st.omega.first * st.omega.first * s
        - mass * g * sin(st.theta.second);

    derivative.omega.second /= denominator;

    return derivative;
    }

   
    phys_obj::state rk4(const phys_obj::state& st, const phys_obj::system& ss, const double dt) noexcept {
        
    /**
     * @brief Runge-Kutta 4th order method for solving 2nd order ODE's.
     * @param st current state of double pendulum
     * @param ss system parameters
     * @param dt time step
     * @return next state of double pendulum
     * 
     * The method implemented is the Runge-Kutta 4th order method with a constant time step.
     * The method is stable and accurate for small time steps.
     */

    phys_obj::state dydx = derive(st, ss);
    phys_obj::state k1   = dt * dydx;
    phys_obj::state yt   = st + 0.5 * k1;

    dydx = derive(yt, ss);
    phys_obj::state k2   = dt * dydx;
    yt   = st + 0.5 * k2;

    dydx = derive(yt, ss);
    phys_obj::state k3   = dt * dydx;
    yt   = st + k3;

    dydx = derive(yt, ss);
    phys_obj::state k4   = dt * dydx;
    
    return 
        st + (1.0 / 6) * k1 + (1.0 / 3) * k2 + (1.0 / 3) * k3 + (1.0 / 6) * k4;
    }

    phys_obj::state advance(const phys_obj::state& st, const phys_obj::system& ss, double time, const double dt) noexcept {
       
    /**
     * @brief Advance double pendulum in time by a given time interval with a given time step.
     */
    double passed = 0.0;
    phys_obj::state ret = st;

    while (passed < time) {
        ret = rk4(ret, ss, dt);
        passed += dt;
    }

    return ret;
    }

}

