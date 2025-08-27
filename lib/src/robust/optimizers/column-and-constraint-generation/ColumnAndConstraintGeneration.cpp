//
// Created by henri on 11.12.24.
//
#include "idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(const Robust::Description &t_robust_description,
                                                                           const Bilevel::Description &t_bilevel_description)
    : m_robust_description(t_robust_description),
      m_bilevel_description(t_bilevel_description) {

}

idol::Robust::ColumnAndConstraintGeneration::ColumnAndConstraintGeneration(
        const idol::Robust::ColumnAndConstraintGeneration &t_src)
        : OptimizerFactoryWithDefaultParameters<ColumnAndConstraintGeneration>(*this),
          m_robust_description(t_src.m_robust_description),
          m_bilevel_description(t_src.m_bilevel_description),
          m_initial_scenarios(t_src.m_initial_scenarios),
          m_master_optimizer(t_src.m_master_optimizer ? t_src.m_master_optimizer->clone() : nullptr),
          m_initial_scenario_by_minimization(t_src.m_initial_scenario_by_minimization ? t_src.m_initial_scenario_by_minimization->clone() : nullptr),
          m_initial_scenario_by_maximization(t_src.m_initial_scenario_by_maximization ? t_src.m_initial_scenario_by_maximization->clone() : nullptr),
          m_check_for_repeated_scenarios(t_src.m_check_for_repeated_scenarios),
          m_inexact_ccg_parameters(t_src.m_inexact_ccg_parameters)
          {

    for (const auto& optimizer : t_src.m_optimizer_feasibility_separation) {
        m_optimizer_feasibility_separation.emplace_back(optimizer->clone());
    }

    for (const auto& optimizer : t_src.m_optimizer_optimality_separation) {
        m_optimizer_optimality_separation.emplace_back(optimizer->clone());
    }

    for (const auto& optimizer : t_src.m_optimizer_joint_separation) {
        m_optimizer_joint_separation.emplace_back(optimizer->clone());
    }

}

idol::OptimizerFactory *idol::Robust::ColumnAndConstraintGeneration::clone() const {
    return new ColumnAndConstraintGeneration(*this);
}

idol::Robust::ColumnAndConstraintGeneration &idol::Robust::ColumnAndConstraintGeneration::with_master_optimizer(
        const idol::OptimizerFactory &t_deterministic_optimizer) {

    if (m_master_optimizer) {
        throw Exception("Master optimizer already set");
    }

    m_master_optimizer.reset(t_deterministic_optimizer.clone());

    return *this;
}

idol::Optimizer *idol::Robust::ColumnAndConstraintGeneration::operator()(const idol::Model &t_model) const {

    if (!m_master_optimizer) {
        throw Exception("Master optimizer not set");
    }

    if (m_optimizer_feasibility_separation.empty() && m_optimizer_optimality_separation.empty() && m_optimizer_joint_separation.empty()) {
        throw Exception("At least one of feasibility, optimality or joint separation optimizers must be set");
    }

    double initial_master_tolerance = 0;
    double update_factor = .5;
    double inexact_rel_gap_ratio = .0;
    if (m_inexact_ccg_parameters.has_value()) {
        initial_master_tolerance = std::get<0>(*m_inexact_ccg_parameters);
        update_factor = std::get<1>(*m_inexact_ccg_parameters);
        inexact_rel_gap_ratio = std::get<2>(*m_inexact_ccg_parameters);
    }

    auto* result = new Optimizers::Robust::ColumnAndConstraintGeneration(t_model,
                                                                         m_robust_description,
                                                                         m_bilevel_description,
                                                                         *m_master_optimizer,
                                                                         m_initial_scenarios,
                                                                         m_initial_scenario_by_minimization ? m_initial_scenario_by_minimization->clone() : nullptr,
                                                                         m_initial_scenario_by_maximization ? m_initial_scenario_by_maximization->clone() : nullptr,
                                                                         m_optimizer_feasibility_separation,
                                                                         m_optimizer_optimality_separation,
                                                                         m_optimizer_joint_separation,
                                                                         m_check_for_repeated_scenarios.value_or(false),
                                                                         initial_master_tolerance,
                                                                         update_factor,
                                                                         inexact_rel_gap_ratio
                                                                         );

    handle_default_parameters(result);

    return result;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::reserve_initial_scenarios(unsigned int t_n) {
    m_initial_scenarios.reserve(t_n);
    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::add_initial_scenario(idol::Point<idol::Var> t_scenario) {
    m_initial_scenarios.emplace_back(std::move(t_scenario));
    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_initial_scenario_by_minimization(const OptimizerFactory& t_optimizer) {

    if (m_initial_scenario_by_minimization) {
        throw Exception("Initial scenario by minimization already set");
    }

    m_initial_scenario_by_minimization.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_initial_scenario_by_maximization(const OptimizerFactory& t_optimizer) {

    if (m_initial_scenario_by_maximization) {
        throw Exception("Initial scenario by maximization already set");
    }

    m_initial_scenario_by_maximization.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::add_feasibility_separation_optimizer(
        const OptimizerFactory &t_optimizer) {

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Feasibility separation optimizer must be a bilevel optimizer");
    }

    m_optimizer_feasibility_separation.emplace_back(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::add_optimality_separation_optimizer(
        const OptimizerFactory &t_optimizer) {

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Optimality separation optimizer must be a bilevel optimizer");
    }

    m_optimizer_optimality_separation.emplace_back(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::add_joint_separation_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Joint separation optimizer must be a bilevel optimizer");
    }

    m_optimizer_joint_separation.emplace_back(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_check_for_repeated_scenarios(bool t_value) {

    if (m_check_for_repeated_scenarios.has_value()) {
        throw Exception("Checking for repeated scenarios has already been configured.");
    }

    m_check_for_repeated_scenarios = t_value;

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::with_inexactness_scheme(double t_initial_master_tolerance,
                                                                     double t_update_factor,
                                                                     double t_inexact_rel_gap_ratio) {

    if (m_inexact_ccg_parameters.has_value()) {
        throw Exception("Inexact scheme parameters have already been configured.");
    }

    if (t_inexact_rel_gap_ratio < 0 || t_inexact_rel_gap_ratio > 1) {
        throw Exception("Parameter \"inexact_rel_gap_ratio\" must be between 0 and 1.");
    }

    m_inexact_ccg_parameters = { t_initial_master_tolerance, t_update_factor, t_inexact_rel_gap_ratio };

    return *this;
}
