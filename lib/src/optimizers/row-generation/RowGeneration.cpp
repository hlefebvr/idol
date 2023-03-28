//
// Created by henri on 06/03/23.
//
#include "optimizers/row-generation/RowGeneration.h"
#include "optimizers/Logger.h"
#include "modeling/objects/Versions.h"

RowGeneration::RowGeneration(const BlockModel<Var> &t_model) : Algorithm(t_model) {

    m_master.reset(t_model.master().clone());

    const unsigned int n_blocks = t_model.n_blocks();
    m_subproblems.reserve(n_blocks);
    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        m_subproblems.emplace_back(*this, i);
    }

}

void RowGeneration::add(const Var &t_var) {
    throw Exception("Not implemented add_var");
}

void RowGeneration::add(const Ctr &t_ctr) {
    throw Exception("Not implemented add_ctr");
}

void RowGeneration::remove(const Var &t_var) {
    throw Exception("Not implemented remove_var");
}

void RowGeneration::remove(const Ctr &t_ctr) {
    throw Exception("Not implemented remove_ctr");
}

void RowGeneration::update() {
    throw Exception("Not implemented update");
}

void RowGeneration::write(const std::string &t_name) {
    m_master->write(t_name);
}

void RowGeneration::hook_before_optimize() {

    set_best_bound(-Inf);
    set_best_obj(+Inf);
    m_iter_lower_bound = -Inf;
    m_iter_upper_bound = +Inf;
    m_iteration_count = 0;
    m_n_generated_rows_at_last_iteration = 0;

    for (auto& subproblem : m_subproblems) {
        subproblem.hook_before_solve();
    }

    Algorithm::hook_before_optimize();
}

void RowGeneration::hook_optimize() {

    main_loop(true);

    log_master_solution(true);

}

void RowGeneration::main_loop(bool t_solve_master) {

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

void RowGeneration::solve_master_problem() {
    m_master->optimize();
}

void RowGeneration::analyze_master_problem_solution() {

    auto status = m_master->get(Attr::Solution::Status);

    set_status(status);

    if (status == Optimal) {

        double& iter_bound = m_sense == Minimize ? m_iter_lower_bound : m_iter_upper_bound;

        iter_bound = m_master->get(Attr::Solution::ObjVal);

        if (!m_is_nested) {

            if (m_sense == Minimize) {
                set_best_bound(std::max(m_iter_lower_bound, get(Attr::Solution::BestBound)));
            } else {
                set_best_obj(std::min(m_iter_upper_bound, get(Attr::Solution::BestObj)));
            }

        }

        m_current_primal_solution = save(*m_master, Attr::Solution::Primal);

        return;

    }

    if (status == Infeasible) {
        set_reason(Proved);
        idol_Log(Trace, RowGeneration, "Terminate. Infeasible master problem.");
        terminate();
        return;
    }

    if (status == Unbounded) {
        set_reason(Proved);
        idol_Log(Trace, RowGeneration, "Terminate. Unbounded master problem.");
        terminate();
        return;
    }

    set_status(Fail);
    set_reason(NotSpecified);
    idol_Log(Trace, RowGeneration, "Terminate. Master problem could not be solved to optimality.");
    terminate();

}

void RowGeneration::log_master_solution(bool t_force) const {


    if (!t_force && m_iteration_count % 1 /* get(Param::RowGeneration::LogFrequency) */ != 0) {
        return;
    }

    idol_Log(Info,
             RowGeneration,
             "<Type=Master> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << parent().time().count() << "> "
             << "<TimI=" << m_master->time().count() << "> "
             << "<Stat=" << (SolutionStatus) m_master->get(Attr::Solution::Status) << "> "
             << "<Reas=" << (SolutionReason) m_master->get(Attr::Solution::Reason) << "> "
             << "<ObjV=" << m_master->get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_rows_at_last_iteration << "> "
             << "<BestBnd=" << best_bound() << "> "
             << "<BestObj=" << best_obj() << "> "
             << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
             << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

bool RowGeneration::stopping_condition() const {
    return get(Attr::Solution::AbsGap) <= ToleranceForAbsoluteGapPricing
           || get(Attr::Solution::RelGap) <= ToleranceForRelativeGapPricing
           || parent().remaining_time() == 0;
}

void RowGeneration::update_subproblems() {

    for (auto& subproblem : m_subproblems) {
        subproblem.update(m_current_primal_solution.value());
    }

}

void RowGeneration::solve_subproblems() {

    for (auto& subproblem : m_subproblems) {

        subproblem.solve();

        if (is_terminated()) { break; }
    }

}

void RowGeneration::log_subproblems_solution() const {

    for (auto& subproblem : m_subproblems) {

        log_subproblem_solution(subproblem);

    }

}

void RowGeneration::log_subproblem_solution(const RowGenerationSP &t_subproblem, bool t_force) const {

    if (!t_force && m_iteration_count % 1 != 0) {
        return;
    }

    const auto& separation = t_subproblem.model();

    idol_Log(Debug,
             RowGeneration,
             "<Type=Separation> "
             << "<Iter=" << m_iteration_count << "> "
             << "<TimT=" << parent().time().count() << "> "
             << "<TimI=" << separation.time().count() << "> "
             << "<Stat=" << (SolutionStatus) separation.get(Attr::Solution::Status) << "> "
             << "<Reas=" << (SolutionReason) separation.get(Attr::Solution::Reason) << "> "
             << "<ObjV=" << separation.get(Attr::Solution::ObjVal) << "> "
             << "<NGen=" << m_n_generated_rows_at_last_iteration << "> "
             << "<BestBnd=" << best_bound() << "> "
             << "<BestObj=" << best_obj() << "> "
             << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
             << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

void RowGeneration::analyze_subproblems_solution() {

    if (!m_current_primal_solution) {
        throw Exception("No master solution saved.");
    }

    bool current_subproblems_are_all_feasible = true;
    double upper_bound = m_current_primal_solution->objective_value();

    for (auto& subproblem : m_subproblems) {

        const auto status = subproblem.model().get(Attr::Solution::Status);

        if (status == Optimal) {
            upper_bound += subproblem.model().get(Attr::Solution::ObjVal);
        } else if (status == Unbounded) {
            current_subproblems_are_all_feasible = false;
            break;
        } else {
            set_status(status);
            set_reason(Proved);
            idol_Log(Trace,
                     RowGeneration,
                     "Terminate. Could not solve subproblem to optimality.");
            terminate();
            return;
        }

    }

    if (current_subproblems_are_all_feasible) {

        double& iter_bound = m_sense == Minimize ? m_iter_upper_bound : m_iter_lower_bound;
        iter_bound = upper_bound;

        if (!m_is_nested) {
            if (m_sense == Minimize) {
                set_best_obj(std::min(best_obj(), m_iter_upper_bound));
            } else {
                set_best_bound(std::max(best_bound(), m_iter_lower_bound));
            }
        }

    }

}

void RowGeneration::clean_up() {
    // TODO
}

void RowGeneration::enrich_master_problem() {

    m_n_generated_rows_at_last_iteration = 0;

    for (auto &subproblem: m_subproblems) {

        bool can_enrich_master;
        if (m_sense == Maximize) {
            can_enrich_master = subproblem.model().get(Attr::Solution::ObjVal) < 0;
        } else {
            can_enrich_master = subproblem.model().get(Attr::Solution::ObjVal) > 0;
        }

        if (can_enrich_master) {
            subproblem.enrich_master_problem();
            ++m_n_generated_rows_at_last_iteration;
        }

    }

}

const BlockModel<Var> &RowGeneration::parent() const {
    return dynamic_cast<const BlockModel<Var>&>(Backend::parent());
}

void RowGeneration::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    const unsigned int subproblem_id = t_var.get(parent().axis());

    if (subproblem_id == MasterId) {
        m_master->set(t_attr, t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Lb) {
        m_subproblems[subproblem_id].apply_lb(t_var, t_value);
        return;
    }

    if (t_attr == Attr::Var::Ub) {
        m_subproblems[subproblem_id].apply_ub(t_var, t_value);
        return;
    }

    Algorithm::set(t_attr, t_var, t_value);
}
