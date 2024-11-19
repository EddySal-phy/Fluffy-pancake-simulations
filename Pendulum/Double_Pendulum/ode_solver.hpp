#ifndef ODE_SOLVER_HPP
#define ODE_SOLVER_HPP

#include <utility>
#include <cmath>
#include "pendulum.hpp"


namespace dp {

    // Operators Overloading
    std::pair<double, double> operator+(const std::pair<double, double>& p1, const std::pair<double, double>& p2) noexcept;
    std::pair<double, double> operator*(const double d, const std::pair<double, double>& p) noexcept;
    phys_obj::state operator+(const phys_obj::state& s1, const phys_obj::state& s2) noexcept;
    phys_obj::state operator*(const double d, const phys_obj::state& s) noexcept;

    // Functions for ODE solver
    phys_obj::state derive(const phys_obj::state& st, const phys_obj::system& ss) noexcept;
    phys_obj::state rk4(const phys_obj::state& st, const phys_obj::system& ss, const double dt)  noexcept;
    phys_obj::state advance(const phys_obj::state& st, const phys_obj::system& ss, double time, const double dt) noexcept;
}

#endif