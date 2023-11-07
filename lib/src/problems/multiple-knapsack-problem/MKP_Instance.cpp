//
// Created by henri on 12/10/22.
//
#include "idol/problems/multiple-knapsack-problem/MKP_Instance.h"
#include <cassert>
#include <fstream>

idol::Problems::MKP::Instance::Instance(unsigned int t_n_knapsacks, unsigned int t_n_items) {

    assert(t_n_knapsacks >= 1);
    assert(t_n_items > 1);

    m_profits.resize(t_n_items);
    m_weights.resize(t_n_items);
    m_capacities.resize(t_n_knapsacks);
}

idol::Problems::MKP::Instance idol::Problems::MKP::read_instance(const std::string& t_filename) {

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file.");
    }

    unsigned int n_knapsacks, n_items;
    double x;

    file >> n_knapsacks >> n_items;

    Instance result(n_knapsacks, n_items);

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        file >> x;
        result.set_profit(j, x);
    }

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        file >> x;
        result.set_weight(j, x);
    }

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        file >> x;
        result.set_capacity(i, x);
    }

    file.close();

    return result;
}
