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
          m_initial_scenario_by_minimization(t_src.m_initial_scenario_by_minimization ? t_src.m_initial_scenario_by_minimization->clone() : nullptr),
          m_initial_scenario_by_maximization(t_src.m_initial_scenario_by_maximization ? t_src.m_initial_scenario_by_maximization->clone() : nullptr),
          m_optimizer_feasibility_separation(t_src.m_optimizer_feasibility_separation ? t_src.m_optimizer_feasibility_separation->clone() : nullptr),
          m_optimizer_optimality_separation(t_src.m_optimizer_optimality_separation ? t_src.m_optimizer_optimality_separation->clone() : nullptr) {

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

    if (!m_optimizer_feasibility_separation && !m_optimizer_optimality_separation) {
        throw Exception("At least one of feasibility or optimality separation optimizers must be set");
    }

    auto* result = new Optimizers::Robust::ColumnAndConstraintGeneration(t_model,
                                                                         m_robust_description,
                                                                         m_bilevel_description,
                                                                         *m_master_optimizer,
                                                                         m_initial_scenarios,
                                                                         m_initial_scenario_by_minimization ? m_initial_scenario_by_minimization->clone() : nullptr,
                                                                         m_initial_scenario_by_maximization ? m_initial_scenario_by_maximization->clone() : nullptr,
                                                                         m_optimizer_feasibility_separation ? m_optimizer_feasibility_separation->clone() : nullptr,
                                                                         m_optimizer_optimality_separation ? m_optimizer_optimality_separation->clone() : nullptr
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

    if (m_optimizer_feasibility_separation) {
        throw Exception("Feasibility separation optimizer already set");
    }

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Feasibility separation optimizer must be a bilevel optimizer");
    }

    m_optimizer_feasibility_separation.reset(t_optimizer.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintGeneration &
idol::Robust::ColumnAndConstraintGeneration::add_optimality_separation_optimizer(
        const OptimizerFactory &t_optimizer) {

    if (m_optimizer_optimality_separation) {
        throw Exception("Optimality separation optimizer already set");
    }

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Optimality separation optimizer must be a bilevel optimizer");
    }

    m_optimizer_optimality_separation.reset(t_optimizer.clone());

    return *this;
}
