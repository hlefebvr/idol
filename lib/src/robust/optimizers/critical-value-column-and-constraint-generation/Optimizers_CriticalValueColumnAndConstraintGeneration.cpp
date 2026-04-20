//
// Created by Henri on 17/04/2026.
//
#include "idol/robust/optimizers/critical-value-column-and-constraint-generation/Optimizers_CriticalValueColumnAndConstraintGeneration.h"

idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(const Model& t_model, const idol::Robust::Description& t_description, const OptimizerFactory& t_master_optimizer, const OptimizerFactory& t_deterministic_optimizer) :
    Algorithm(t_model),
    m_description(t_description),
    m_master_optimizer_factory(t_master_optimizer.clone()),
    m_deterministic_optimizer_factory(t_deterministic_optimizer.clone()) {

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::hook_optimize() {

    m_formulation = std::make_unique<CVCCG::Formulation>(*this);

    do {

        solve_master_problem();

        analyze_master_problem();

        solve_sub_problems();

        if (check_stopping_criterion()) { break; }

    } while (true);

    throw Exception("STOPPED");
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::solve_master_problem() {
    m_formulation->master().optimize();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::analyze_master_problem() {

    const auto& master = m_formulation->master();
    const auto status = master.get_status();
    const auto reason = master.get_reason();

    if (status != Optimal) {
        set_status(status);
        set_reason(reason);
        terminate();
        return;
    }

    const double objective_value = master.get_best_obj();

    set_best_obj(objective_value);

}

bool idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::check_stopping_criterion() {
    return true;
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::solve_sub_problems() {

    const auto& master = m_formulation->master();
    const auto& master_solution = save_primal(master);
    const unsigned int n_uncertainties = m_formulation->n_uncertainties();

    m_formulation->update_sub_problem_constraints(master_solution);

    std::list<PrimalPoint> scenarios;
    for (unsigned int i = 0; i < n_uncertainties; ++i) {

        m_formulation->update_sub_problem_objective(master_solution, i);

        auto& sub_problem = m_formulation->sub_problem();
        sub_problem.optimize();

        auto scenario = save_primal(sub_problem);

        if (scenario.status() != Optimal) {
            set_status(scenario.status());
            set_reason(scenario.reason());
            terminate();
            return;
        }

        if (-scenario.objective_value() > get_tol_feasibility()) {
            scenarios.emplace_back(std::move(scenario));
        }

    }

    // TODO: should apply unique() to avoid adding twice the same scenario

    for (const auto& scenario : scenarios) {
        m_formulation->add_scenario(scenario);
    }

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::write(const std::string& t_name) {
    throw Exception("Not implemented write");
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::set_solution_index(unsigned t_index) {
    throw Exception("Not implemented set_solution_index");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_primal(const Var& t_var) const {
    throw Exception("Not implemented get_var_primal");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_reduced_cost(const Var& t_var) const {
    throw Exception("Not implemented get_var_reduced_cost");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_ray(const Var& t_var) const {
    throw Exception("Not implemented get_var_ray");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_ctr_dual(const Ctr& t_ctr) const {
    throw Exception("Not implemented get_ctr_dual");
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_ctr_farkas(const Ctr& t_ctr) const {
    throw Exception("Not implemented get_ctr_farkas");
}

unsigned idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_n_solutions() const {
    throw Exception("Not implemented get_n_solutions");
}

unsigned idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_solution_index() const {
    return 0;
}
