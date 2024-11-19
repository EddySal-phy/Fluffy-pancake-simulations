#ifndef SYSTEM_HPP
#define SYSTEM_HPP
#include "random_generator.hpp"
#include <vector>

namespace sys{

    // Array of Ising spins
    void init_syst(std::vector<std::vector<int>>& arr, int n,  int m);

    // Monte Carlo move using Metropolis algorithm
    void mcmove(std::vector<std::vector<int>>& config, double beta);

    // Calculate the magnetization
    double calculate_magnetization(const std::vector<std::vector<int>>& config);
    double autocorrelation(const std::vector<double>& mag_vec, int h);
    double mean(const std::vector<double>& data);
}

#endif