//
// Created by henri on 12/10/22.
//
#include "../../../include/problems/GAP/GAP_Instance.h"
#include <cassert>
#include <fstream>
#include <random>

Problems::GAP::Instance::Instance(unsigned int t_n_knapsacks, unsigned int t_n_items) {

    assert(t_n_knapsacks > 1);
    assert(t_n_items > 1);

    m_costs.resize(t_n_knapsacks);
    for (unsigned int i = 0 ; i < t_n_knapsacks ; ++i) {
        m_costs[i].resize(t_n_items);
    }

    m_resource_consumptions.resize(t_n_knapsacks);
    for (unsigned int i = 0 ; i < t_n_knapsacks ; ++i) {
        m_resource_consumptions[i].resize(t_n_items);
    }

    m_capacities.resize(t_n_knapsacks);

}

Problems::GAP::Instance Problems::GAP::read_instance(const std::string& t_filename) {

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file.");
    }

    unsigned int n_knapsacks, n_items;
    double x;

    file >> n_knapsacks >> n_items;

    Instance result(n_knapsacks, n_items);

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            file >> x;
            result.set_cost(i, j, x);
        }
    }

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            file >> x;
            result.set_resource_consumption(i, j, x);
        }
    }

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        file >> x;
        result.set_capacity(i, x);
    }

    file.close();

    return result;
}

Problems::GAP::Instance
generate_instance_Chu_and_Beasley_1997_C(unsigned int t_n_agents, unsigned int t_n_jobs) {

    Problems::GAP::Instance result(t_n_agents, t_n_jobs);

    std::random_device rd;
    std::mt19937 engine(rd());

    auto r_dist = std::uniform_int_distribution<int>(5, 25);
    auto c_dist = std::uniform_int_distribution<int>(10, 50);

    for (unsigned int i = 0 ; i < t_n_agents ; ++i) {
        for (unsigned int j = 0 ; j < t_n_jobs ; ++j) {
            result.set_resource_consumption(i, j, r_dist(engine));
            result.set_cost(i, j, c_dist(engine));
        }
    }

    for (unsigned int i = 0 ; i < t_n_agents ; ++i) {
        double sum_r_ij = 0;
        for (unsigned int j = 0 ; j < t_n_jobs ; ++j) {
            sum_r_ij += result.resource_consumption(i, j);
        }
        result.set_capacity(i, .8 * sum_r_ij / t_n_agents);
    }

    return result;
}