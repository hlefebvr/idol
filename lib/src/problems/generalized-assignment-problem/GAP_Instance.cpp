//
// Created by henri on 12/10/22.
//
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include <cassert>
#include <fstream>
#include <random>

idol::Problems::GAP::Instance::Instance(unsigned int t_n_agents, unsigned int t_n_jobs) {

    assert(t_n_agents > 1);
    assert(t_n_jobs > 1);

    m_costs.resize(t_n_agents);
    for (unsigned int i = 0 ; i < t_n_agents ; ++i) {
        m_costs[i].resize(t_n_jobs);
    }

    m_resource_consumptions.resize(t_n_agents);
    for (unsigned int i = 0 ; i < t_n_agents ; ++i) {
        m_resource_consumptions[i].resize(t_n_jobs);
    }

    m_capacities.resize(t_n_agents);

}

idol::Problems::GAP::Instance idol::Problems::GAP::read_instance(const std::string& t_filename) {

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file.");
    }

    unsigned int n_agents, n_jobs;
    double x;

    file >> n_agents >> n_jobs;

    Instance result(n_agents, n_jobs);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        for (unsigned int j = 0 ; j < n_jobs ; ++j) {
            file >> x;
            result.set_cost(i, j, x);
        }
    }

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        for (unsigned int j = 0 ; j < n_jobs ; ++j) {
            file >> x;
            result.set_resource_consumption(i, j, x);
        }
    }

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        file >> x;
        result.set_capacity(i, x);
    }

    file.close();

    return result;
}

std::ostream& idol::Problems::GAP::operator<<(std::ostream& t_os, const idol::Problems::GAP::Instance& t_instance) {

    const unsigned int n_agents = t_instance.n_agents();
    const unsigned int n_jobs = t_instance.n_jobs();

    t_os << n_agents << '\t' << n_jobs << '\n';

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        for (unsigned int j = 0; j < n_jobs; ++j) {
            t_os << t_instance.cost(i,j) << '\t';
        }
        t_os << '\n';
    }

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        for (unsigned int j = 0; j < n_jobs; ++j) {
            t_os << t_instance.resource_consumption(i,j) << '\t';
        }
        t_os << '\n';
    }

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        t_os << t_instance.capacity(i) << '\t';
    }

    return t_os;
}

idol::Problems::GAP::Instance
idol::Problems::GAP::generate_instance_Chu_and_Beasley_1997_C(unsigned int t_n_agents, unsigned int t_n_jobs) {

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