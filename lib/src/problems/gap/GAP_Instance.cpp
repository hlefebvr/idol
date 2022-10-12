//
// Created by henri on 12/10/22.
//
#include "../../../include/problems/gap/GAP_Instance.h"
#include <cassert>
#include <fstream>

ProblemSpecific::GAP::Instance::Instance(unsigned int t_n_knapsacks, unsigned int t_n_items) {

    assert(t_n_knapsacks > 1);
    assert(t_n_items > 1);

    m_p.resize(t_n_knapsacks);
    for (unsigned int i = 0 ; i < t_n_knapsacks ; ++i) {
        m_p[i].resize(t_n_items);
    }

    m_w.resize(t_n_knapsacks);
    for (unsigned int i = 0 ; i < t_n_knapsacks ; ++i) {
        m_w[i].resize(t_n_items);
    }

    m_t.resize(t_n_knapsacks);

}

ProblemSpecific::GAP::Instance ProblemSpecific::GAP::read_instance(const std::string& t_filename) {

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
            result.set_p(i, j, x);
        }
    }

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            file >> x;
            result.set_w(i, j, x);
        }
    }

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        file >> x;
        result.set_t(i, x);
    }

    file.close();

    return result;
}