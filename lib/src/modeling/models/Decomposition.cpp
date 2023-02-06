//
// Created by henri on 06/02/23.
//
#include "modeling/models/Decomposition.h"

template<typename AxisT>
Decomposition<AxisT>::Decomposition(Env& t_env, unsigned int t_n_subproblems)
        : m_axis_subproblem(t_env),
          m_opposite_axis_subproblem(t_env),
          m_master_problem(t_env) {

    m_subproblems.reserve(t_n_subproblems);
    for (unsigned int i = 0 ; i < t_n_subproblems ; ++i) {
        m_subproblems.emplace_back(t_env);
    }

}

template<typename AxisT>
void Decomposition<AxisT>::set_epigraph(unsigned int t_index, const AxisT &t_axis) {
    if (!m_master_problem.has(t_axis)) {
        throw Exception("Epigraph must be part of master.");
    }
    m_epigraphs[t_index] = t_axis;
}
