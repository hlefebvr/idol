//
// Created by henri on 13/10/22.
//
#include "../../../include/algorithms/callbacks/AlgorithmInCallback.h"

AlgorithmInCallback::AlgorithmInCallback(Algorithm &t_rmp_solution_strategy) : m_algorithm(t_rmp_solution_strategy) {

}

Solution::Primal AlgorithmInCallback::primal_solution() const {
    return m_is_in_callback ? m_context->node_primal_solution() : m_algorithm.primal_solution();
}

Ctr AlgorithmInCallback::add_ctr(TempCtr&& t_temporary_constraint) {
    if (m_is_in_callback) {
        return m_context->add_lazy_cut(std::move(t_temporary_constraint));
    }
    return m_algorithm.add_ctr(std::move(t_temporary_constraint));
}

void AlgorithmInCallback::remove(const Ctr &t_constraint) {
    if (m_is_in_callback) {
        throw Exception("Cannot remove constraint within callback.");
    }
    m_algorithm.remove(t_constraint);
}

void AlgorithmInCallback::set(const Parameter<double> &t_param, double t_value) {
    m_algorithm.set(t_param, t_value);
}

void AlgorithmInCallback::set(const Parameter<bool> &t_param, bool t_value) {
    m_algorithm.set(t_param, t_value);
}

void AlgorithmInCallback::set(const Parameter<int> &t_param, int t_value) {
    m_algorithm.set(t_param, t_value);
}

double AlgorithmInCallback::get(const Parameter<double> &t_param) const {
    return m_algorithm.get(t_param);
}

bool AlgorithmInCallback::get(const Parameter<bool> &t_param) const {
    return m_algorithm.get(t_param);
}

int AlgorithmInCallback::get(const Parameter<int> &t_param) const {
    return m_algorithm.get(t_param);
}
