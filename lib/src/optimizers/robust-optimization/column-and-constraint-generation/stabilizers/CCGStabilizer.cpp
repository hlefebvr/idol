//
// Created by henri on 06.03.24.
//
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/CCGStabilizer.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

void idol::Robust::CCGStabilizer::Strategy::set_status(SolutionStatus t_status) {
    m_parent->set_status(t_status);
}

void idol::Robust::CCGStabilizer::Strategy::set_reason(idol::SolutionReason t_reason) {
    m_parent->set_reason(t_reason);
}

void idol::Robust::CCGStabilizer::Strategy::set_best_obj(double t_value) {
    m_parent->set_best_obj(t_value);
}

void idol::Robust::CCGStabilizer::Strategy::set_best_bound(double t_value) {
    m_parent->set_best_bound(t_value);
}

void idol::Robust::CCGStabilizer::Strategy::terminate() {
    m_parent->terminate();
}

const idol::Model &idol::Robust::CCGStabilizer::Strategy::master_problem() const {
    return m_parent->m_master_problem;
}

idol::Model &idol::Robust::CCGStabilizer::Strategy::master_problem() {
    return m_parent->m_master_problem;
}

const std::list<idol::Var> &idol::Robust::CCGStabilizer::Strategy::upper_level_vars() const {
    return m_parent->m_upper_level_variables_list;
}

const idol::Solution::Primal &idol::Robust::CCGStabilizer::Strategy::current_separation_solution() const {
    return *m_parent->m_current_separation_solution;
}

const idol::Solution::Primal &idol::Robust::CCGStabilizer::Strategy::current_master_solution() const {
    return *m_parent->m_current_master_solution;
}

void idol::Robust::CCGStabilizer::Strategy::check_stopping_condition() const {
    m_parent->check_stopping_condition();
}

