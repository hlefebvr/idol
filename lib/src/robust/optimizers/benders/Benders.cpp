//
// Created by henri on 08.05.25.
//
#include "idol/robust/optimizers/benders/Benders.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"
#include "idol/robust/optimizers/benders/Optimizers_Benders.h"

idol::Robust::Benders::Benders(const Robust::Description &t_robust_description,
                               const Bilevel::Description &t_bilevel_description)
                               : OptimizerFactoryWithDefaultParameters(),
                                 m_robust_description(t_robust_description),
                                 m_bilevel_description(t_bilevel_description) {

}

idol::Robust::Benders::Benders(const idol::Robust::Benders &t_src)
    : OptimizerFactoryWithDefaultParameters(t_src),
      m_robust_description(t_src.m_robust_description),
      m_bilevel_description(t_src.m_bilevel_description),
      m_initial_scenarios(t_src.m_initial_scenarios) {

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

idol::OptimizerFactory *idol::Robust::Benders::clone() const {
    return new Benders(*this);
}

idol::Robust::Benders &idol::Robust::Benders::reserve_initial_scenarios(unsigned int t_n) {

    m_initial_scenarios.reserve(t_n);

    return *this;
}

idol::Robust::Benders &idol::Robust::Benders::add_initial_scenario(idol::Point<idol::Var> t_scenario) {
    m_initial_scenarios.emplace_back(std::move(t_scenario));
    return *this;
}

idol::Robust::Benders &
idol::Robust::Benders::add_feasibility_separation_optimizer(const idol::OptimizerFactory &t_optimizer) {

    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Feasibility separation optimizer must be a Bilevel::OptimizerInterface");
    }

    m_optimizer_feasibility_separation.emplace_back(t_optimizer.clone());

    return *this;
}

idol::Robust::Benders &
idol::Robust::Benders::add_optimality_separation_optimizer(const idol::OptimizerFactory &t_optimizer) {
    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Optimality separation optimizer must be a Bilevel::OptimizerInterface");
    }

    m_optimizer_optimality_separation.emplace_back(t_optimizer.clone());

    return *this;
}

idol::Robust::Benders &
idol::Robust::Benders::add_joint_separation_optimizer(const idol::OptimizerFactory &t_optimizer) {
    if (!t_optimizer.is<Bilevel::OptimizerInterface>()) {
        throw Exception("Joint separation optimizer must be a Bilevel::OptimizerInterface");
    }

    m_optimizer_joint_separation.emplace_back(t_optimizer.clone());

    return *this;
}

idol::Optimizer *idol::Robust::Benders::operator()(const idol::Model &t_model) const {

    if (m_optimizer_feasibility_separation.empty() && m_optimizer_optimality_separation.empty() && m_optimizer_joint_separation.empty()) {
        throw Exception("At least one of feasibility, optimality or joint separation optimizers must be set");
    }

    auto result = new Optimizers::Robust::Benders(t_model,
                                                  m_robust_description,
                                                  m_bilevel_description,
                                                  m_initial_scenarios,
                                                  m_optimizer_feasibility_separation,
                                                  m_optimizer_optimality_separation,
                                                  m_optimizer_joint_separation);

    handle_default_parameters(result);

    return result;
}

