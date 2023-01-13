//
// Created by henri on 05/01/23.
//
#include "algorithms/benders/Benders.h"
#include "algorithms/branch-and-bound/BranchAndBound_Events.h"
#include "algorithms/callbacks/Algorithm_Events.h"

Benders::Benders(Model &t_model, bool t_is_nested)
    : m_model(t_model), m_is_nested(t_is_nested) {

}

void Benders::initialize() {

    m_sense = m_model.get(Attr::Obj::Sense);
    m_best_lower_bound = -Inf;
    m_best_upper_bound = +Inf;
    m_iter_lower_bound = -Inf;
    m_iter_upper_bound = +Inf;
    m_iteration_count = 0;
    m_n_generated_columns_at_last_iteration = 0;
    m_current_subproblems_are_all_feasible = true;

    if (!m_master_solution_strategy) {
        throw Exception("No solution strategy at hand for solving master problem.");
    }

    for (auto& subproblem : m_subproblems) {
        subproblem.initialize();
    }

}

void Benders::execute() {

    initialize();

    call_callback(Event_::Algorithm::Begin);

    main_loop(true);

    log_master_solution(true);

    call_callback(Event_::Algorithm::End);

}

void Benders::main_loop(bool t_solve_master) {

    do {

        if (t_solve_master) {

            solve_master_problem();

            if (m_is_nested) {
                break;
            }

        }

        analyze_master_problem_solution();

        log_master_solution();

        if (is_terminated() || stopping_condition()) { break; }

        update_subproblems();

        solve_subproblems();

        analyze_subproblems_solution();

        log_subproblems_solution();

        if (is_terminated() || stopping_condition()) { break; }

        clean_up();

        enrich_master_problem();

        ++m_iteration_count;

    } while (!m_is_nested);

}

Solution::Primal Benders::primal_solution() const {
    return m_master_solution_strategy->primal_solution();
}

void Benders::solve_master_problem() {

    m_master_solution_strategy->solve();

}

void Benders::analyze_master_problem_solution() {

    auto status = m_master_solution_strategy->status();

    set_status(status);

    if (status == Optimal) {

        double& iter_bound = m_sense == Minimize ? m_iter_lower_bound : m_iter_upper_bound;

        iter_bound = m_master_solution_strategy->get(Attr::Solution::ObjVal);

        if (!m_is_nested) {

            if (m_sense == Minimize) {
                m_best_lower_bound = std::max(m_iter_lower_bound, m_best_lower_bound);
            } else {
                m_best_upper_bound = std::min(m_iter_upper_bound, m_best_upper_bound);
            }

        }

        m_current_primal_solution = m_master_solution_strategy->primal_solution();

        return;

    }

    if (status == Infeasible) {
        set_reason(Proved);
        idol_Log(Trace, Benders, "Terminate. Infeasible master problem.");
        terminate();
        return;
    }

    if (status == Unbounded) {
        set_reason(Proved);
        idol_Log(Trace, Benders, "Terminate. Unbounded master problem.");
        terminate();
        return;
    }

    set_status(Fail);
    set_reason(NotSpecified);
    idol_Log(Trace, Benders, "Terminate. Master problem could not be solved to optimality.");
    terminate();

}

void Benders::log_master_solution(bool t_force) const {


    if (!t_force && m_iteration_count % 1 != 0) {
        return;
    }

    idol_Log(Info,
             Benders,
             "<Type=Master> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << time().count() << "> "
             << "<TimI=" << m_master_solution_strategy->time().count() << "> "
             << "<Stat=" << status() << "> "
             << "<Reas=" << m_master_solution_strategy->reason() << "> "
             << "<ObjV=" << m_master_solution_strategy->get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
             << "<Lb=" << m_best_lower_bound << "> "
             << "<Ub=" << m_best_upper_bound << "> "
             << "<RGap=" << relative_gap(m_best_lower_bound, m_best_upper_bound) * 100 << "> "
             << "<AGap=" << absolute_gap(m_best_lower_bound, m_best_upper_bound) << "> "
    );


}

bool Benders::stopping_condition() const {
    return absolute_gap(m_best_lower_bound, m_best_upper_bound) <= ToleranceForAbsoluteGapPricing
           || relative_gap(m_best_lower_bound, m_best_upper_bound) <= ToleranceForRelativeGapPricing
           || remaining_time() == 0;
}

void Benders::update_subproblems() {

    for (auto& subproblem : m_subproblems) {
        subproblem.update(m_current_primal_solution.value());
    }

}

void Benders::solve_subproblems() {


    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        if (is_terminated()) { break; }
    }


}

void Benders::log_subproblems_solution() const {

    for (auto& subproblem : m_subproblems) {

        log_subproblem_solution(subproblem);

    }

}

void Benders::analyze_subproblems_solution() {

    if (!m_current_primal_solution) {
        throw Exception("No master solution saved.");
    }

    m_current_subproblems_are_all_feasible = true;
    double upper_bound = m_current_primal_solution->objective_value();

    for (auto& subproblem : m_subproblems) {

        const auto status = subproblem.status();

        if (status == Optimal) {
            upper_bound += subproblem.objective_value();
        } else if (status == Unbounded) {
            m_current_subproblems_are_all_feasible = false;
            break;
        } else {
            set_status(status);
            set_reason(Proved);
            idol_Log(Trace,
                     Benders,
                     "Terminate. Could not solve subproblem to optimality.");
            terminate();
            return;
        }

    }

    if (m_current_subproblems_are_all_feasible) {

        double& iter_bound = m_sense == Minimize ? m_iter_upper_bound : m_iter_lower_bound;
        iter_bound = upper_bound;

        if (!m_is_nested) {
            if (m_sense == Minimize) {
                m_best_upper_bound = std::min(m_best_upper_bound, m_iter_upper_bound);
            } else {
                m_best_lower_bound = std::max(m_best_lower_bound, m_iter_lower_bound);
            }
        }

    }

}

void Benders::clean_up() {
    // TODO
}

void Benders::enrich_master_problem() {

    m_n_generated_columns_at_last_iteration = 0;

    for (auto &subproblem: m_subproblems) {

        bool can_enrich_master = subproblem.objective_value() < 0;

        if (can_enrich_master) {
            subproblem.enrich_master_problem();
            ++m_n_generated_columns_at_last_iteration;
        }

    }

}

void Benders::log_subproblem_solution(const BendersSP &t_subproblem, bool t_force) const {


    if (!t_force && m_iteration_count % 1 != 0) {
        return;
    }

    const auto& separation = t_subproblem.exact_solution_strategy();

    idol_Log(Debug,
             Benders,
             "<Type=Separation> "
                     << "<Iter=" << m_iteration_count << "> "
                     << "<TimT=" << time().count() << "> "
                     << "<TimI=" << separation.time().count() << "> "
                     << "<Stat=" << separation.status() << "> "
                     << "<Reas=" << separation.reason() << "> "
                     << "<ObjV=" << separation.get(Attr::Solution::ObjVal) << "> "
                     << "<NGen=" << m_n_generated_columns_at_last_iteration << "> "
                     << "<Lb=" << m_best_lower_bound << "> "
                     << "<Ub=" << m_best_upper_bound << "> "
                     << "<RGap=" << relative_gap(m_best_lower_bound, m_best_upper_bound) * 100 << "> "
                     << "<AGap=" << absolute_gap(m_best_lower_bound, m_best_upper_bound) << "> "
    );


}

BendersSP &Benders::add_subproblem(Model &t_model, const Ctr &t_cut, const Var &t_epigraph) {
    m_subproblems.emplace_back(*this, m_subproblems.size(), t_model, t_cut, t_epigraph);
    return m_subproblems.back();
}

AttributeManager &Benders::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {

    if (m_model.get(Attr::Var::Status, t_object)) {
        return m_model;
    }

    if (m_subproblems.size() > 1) {
        throw Exception("Accessing subproblem attributes with more than one subproblem is not yet implemented.");
    }
    return m_subproblems.front().model();
}

void Benders::set(const AttributeWithTypeAndArguments<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (m_subproblems.front().model().get(Attr::Var::Status, t_var)) {
        m_subproblems.front().set(t_attr, t_var, t_value);
        return;
    }

    Delegate::set(t_attr, t_var, t_value);
}

void Benders::set(const Parameter<double> &t_param, double t_value) {
    m_master_solution_strategy->set(t_param, t_value);
    Algorithm::set(t_param, t_value);
}

int Benders::sense() const {
    return m_master_solution_strategy->sense();
}
