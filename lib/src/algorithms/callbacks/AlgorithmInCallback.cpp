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

bool AlgorithmInCallback::set_parameter_double(const Parameter<double> &t_param, double t_value) {
    return m_algorithm.set_parameter_double(t_param, t_value);
}

bool AlgorithmInCallback::set_parameter_int(const Parameter<int> &t_param, int t_value) {
    return m_algorithm.set_parameter_int(t_param, t_value);
}

std::optional<double> AlgorithmInCallback::get_parameter_double(const Parameter<double> &t_param) const {
    return m_algorithm.get_parameter_double(t_param);
}

std::optional<int> AlgorithmInCallback::get_parameter_int(const Parameter<int> &t_param) const {
    return m_algorithm.get_parameter_int(t_param);
}
