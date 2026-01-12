//
// Created by henri on 09.01.26.
//
#include <idol/robust/optimizers/critical-value-column-and-constraint-generation-algorithm/Optimizers_CriticalValueColumnAndConstraintGeneration.h>

idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::CriticalValueColumnAndConstraintGeneration(
    const Model& t_parent,
    const idol::Robust::Description& t_description,
    const OptimizerFactory& t_master_optimizer,
    const OptimizerFactory& t_separation_optimizer)
        : Algorithm(t_parent),
          m_robust_description(t_description),
          m_master_optimizer(t_master_optimizer.clone()),
          m_separation_optimizer(t_separation_optimizer.clone())
{
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::add(const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::add(const Ctr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::add(const QCtr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::add(const SOSCtr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::remove(const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::remove(const Ctr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::remove(const QCtr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::write(const std::string& t_name) {
    get_formulation_or_throw().master().write(t_name);
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::set_solution_index(unsigned t_index) {
    get_formulation_or_throw().master().set_solution_index(t_index);
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_obj_sense() {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_obj() {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_rhs() {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_obj_constant() {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_mat_coeff(const Ctr& t_ctr,
    const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_ctr_type(const Ctr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_ctr_rhs(const Ctr& t_ctr) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_var_type(const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_var_lb(const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_var_ub(const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update_var_obj(const Var& t_var) {
    m_formulation.reset();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::update() {
    m_formulation.reset();
}

idol::CVCCG::Formulation& idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::
get_formulation_or_throw() {
    if (!m_formulation) {
        throw Exception("No formulation is currently active. Please, call solve before.");
    }
    return *m_formulation;
}

const idol::CVCCG::Formulation& idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::
get_formulation_or_throw() const {
    return const_cast<CriticalValueColumnAndConstraintGeneration&>(*this).get_formulation_or_throw();
}

std::string idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::name() const {
    return "critical-value-ccg";
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_primal(const Var& t_var) const {
    return get_formulation_or_throw().master().get_var_primal(t_var);
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_reduced_cost(
    const Var& t_var) const {
    return get_formulation_or_throw().master().get_var_reduced_cost(t_var);
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_var_ray(const Var& t_var) const {
    return get_formulation_or_throw().master().get_var_ray(t_var);
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_ctr_dual(const Ctr& t_ctr) const {
    return get_formulation_or_throw().master().get_ctr_dual(t_ctr);
}

double idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_ctr_farkas(const Ctr& t_ctr) const {
    return get_formulation_or_throw().master().get_ctr_farkas(t_ctr);
}

unsigned idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_n_solutions() const {
    return get_formulation_or_throw().master().get_n_solutions();
}

unsigned idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::get_solution_index() const {
    return get_formulation_or_throw().master().get_solution_index();
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    m_formulation = std::make_unique<idol::CVCCG::Formulation>(*this);

    set_best_bound(-Inf);
    set_best_obj(Inf);
    m_n_iterations = 0;
}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::hook_optimize() {

    while (true) {

        solve_master_problem();

        if (is_terminated()) { break; }

        solve_separation_problem();

        if (is_terminated()) { break; }

        add_scenario_to_master_problem();

        // log_iteration();

        ++m_n_iterations;

        if (m_n_iterations > 300) {
            throw Exception("STOP!");
        }
    }

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::hook_after_optimize() {
    Algorithm::hook_after_optimize();
}
void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::solve_master_problem() {

    auto& master_problem = m_formulation->master();

    master_problem.optimize();

    const auto status = master_problem.get_status();
    if (status != Optimal) {
        set_status(status);
        if (status == Feasible) {
            set_best_bound(master_problem.get_best_bound());
        }
        terminate();
        return;
    }

    m_last_master_solution = save_primal(master_problem);
    set_best_bound(master_problem.get_best_bound());

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::solve_separation_problem() {

    m_formulation->update_separation_problems(*m_last_master_solution);

    //std::cout << "MASTER PROBLEM:\n" << m_formulation->master() << std::endl;

// TODO: Parallelize
    for (auto& [ctr, separation_problem] : m_formulation->separation_problems()) {
        separation_problem.optimize();
        //std::cout << "SEPARATION PROBLEM:\n" << separation_problem << std::endl;
    }

    bool all_feasible = true;
    for (const auto& [ctr, separation_problem] : m_formulation->separation_problems()) {

        const auto status = separation_problem.get_status();
        if (status != Optimal) {
            set_status(status);
            set_best_obj(Inf);
            terminate();
            return;
        }

        const double obj = separation_problem.get_best_obj();
        if (obj <= -Tolerance::Feasibility) {
            all_feasible = false;
            break;
        }

    }

    if (!all_feasible) {
        return;
    }

    set_status(Optimal);
    set_best_obj(get_best_bound());
    terminate();

}

void idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration::add_scenario_to_master_problem() {

    for (const auto& [ctr, separation_problem] : m_formulation->separation_problems()) {
        const double obj = separation_problem.get_best_obj();
        if (obj <= -Tolerance::Feasibility) {
            const auto scenario = save_primal(separation_problem);
            m_formulation->add_scenario(ctr, scenario); // this constraint is violated by this scenario
        }
    }

}