//
// Created by henri on 02/03/23.
//
#include "problems/robust-problem/RobustProblem.h"

RobustProblem::RobustProblem(Env& t_env, unsigned int t_n_stages) {

    if (t_n_stages == 0 || t_n_stages > 2) {
        throw Exception("The number of stages must be 1 or 2.");
    }

    m_stages.reserve(t_n_stages);
    m_uncertainty_sets.reserve(std::max<unsigned int>(1, t_n_stages - 1));

    for (unsigned int k = 0 ; k < t_n_stages ; ++k) {
        m_stages.emplace_back(t_env);
        m_uncertainty_sets.emplace_back(t_env);
    }

}

bool RobustProblem::get(const Parameter<bool> &t_param) const {

    if (t_param.is_in_section(Param::Sections::AdjustableRobust)) {
        return m_bool_parameters.get(t_param);
    }

    throw Exception("Parameter " + t_param.name() + " was not expected.");
}

void RobustProblem::set(const Parameter<bool> &t_param, bool t_value) {

    if (t_param.is_in_section(Param::Sections::AdjustableRobust)) {
        if (n_stages() == 2) {
            m_bool_parameters.set(t_param, t_value);
            return;
        } else {
            throw Exception("Parameter " + t_param.name() + " is intended for two-stage problems.");
        }
    }

    throw Exception("Parameter " + t_param.name() + " was not expected.");
}

