//
// Created by henri on 06/02/23.
//
#include "modeling/models/Decomposition.h"

template<typename AxisT>
Decomposition<AxisT>::Decomposition(Env &t_env, unsigned int t_n_subproblems, Annotation<AxisT, unsigned int> t_axis_subproblem)
    : m_axis_subproblem(std::move(t_axis_subproblem)),
      m_opposite_axis_subproblem(Annotation<OppositeAxisT, unsigned int>::make_with_default_value(t_env, m_axis_subproblem.name() + "_opposite", MasterId)),
      m_master_problem(t_env),
      m_pattern(t_n_subproblems),
      m_epigraphs(t_n_subproblems) {

    m_subproblems.reserve(t_n_subproblems);
    for (unsigned int i = 0 ; i < t_n_subproblems ; ++i) {
        m_subproblems.emplace_back(t_env);
    }

}

template<typename AxisT>
Decomposition<AxisT>::Decomposition(Env &t_env, unsigned int t_n_subproblems)
    : Decomposition(t_env, t_n_subproblems, Annotation<AxisT, unsigned int>(t_env, "_decomposition")) {

}

template<typename AxisT>
void Decomposition<AxisT>::set_epigraph(unsigned int t_index, const AxisT &t_axis) {
    if (!m_master_problem.has(t_axis)) {
        throw Exception("Epigraph must be part of master.");
    }
    m_epigraphs[t_index] = t_axis;
}
