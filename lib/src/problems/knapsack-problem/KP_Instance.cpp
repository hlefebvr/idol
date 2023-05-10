//
// Created by henri on 12/10/22.
//
#include "../../../include/problems/knapsack-problem/KP_Instance.h"
#include <cassert>
#include <fstream>

Problems::KP::Instance::Instance(unsigned int t_n_items) {

    assert(t_n_items > 1);

    m_profit.resize(t_n_items);
    m_weight.resize(t_n_items);

}


Problems::KP::Instance Problems::KP::read_instance(const std::string& t_filename) {

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file.");
    }

    unsigned int n_items;
    double x;

    file >> n_items;

    Instance result(n_items);

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        file >> x;
        result.set_profit(j, x);
    }

    for (unsigned int j = 0 ; j < n_items ; ++j) {
        file >> x;
        result.set_weight(j, x);
    }

    file >> x;
    result.set_capacity(x);

    file.close();

    return result;
}


Problems::KP::Instance Problems::KP::read_instance_kplib(const std::string& t_filename) {

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file.");
    }

    unsigned int n_items;
    double x;

    file >> n_items;

    Instance result(n_items);

    file >> x;
    result.set_capacity(x);


    for (unsigned int j = 0 ; j < n_items ; ++j) {

        file >> x;
        result.set_profit(j, x);

        file >> x;
        result.set_weight(j, x);

    }

    file.close();

    return result;
}

