//
// Created by henri on 06.03.24.
//
#include <cassert>
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/TrustRegion.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Robust::CCGStabilizers::TrustRegion::Strategy *idol::Robust::CCGStabilizers::TrustRegion::operator()() const {
    return new Strategy(m_trust_factors.has_value() ? *m_trust_factors : std::vector<double>{ .01, .02, .5 });
}

idol::Robust::CCGStabilizers::TrustRegion *idol::Robust::CCGStabilizers::TrustRegion::clone() const {
    return new TrustRegion(*this);
}

idol::Robust::CCGStabilizers::TrustRegion &
idol::Robust::CCGStabilizers::TrustRegion::with_trust_factors(std::vector<double> t_trust_factors) {

    if (m_trust_factors.has_value()) {
        throw Exception("Trust factors already set.");
    }

    m_trust_factors = std::move(t_trust_factors);

    if (std::any_of(m_trust_factors->begin(), m_trust_factors->end(), [](const double& t_value) { return t_value <= 0; })) {
        throw Exception("Trust factors must be positive.");
    }

    return *this;
}

idol::Robust::CCGStabilizers::TrustRegion::Strategy::Strategy(const std::vector<double> &t_trust_factors) : m_trust_factor(t_trust_factors) {

}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::initialize() {

    const auto& master_problem = this->master_problem();

    remove_all_stabilization_constraints(this->master_problem());
    m_stability_center.reset();
    m_reversed_local_branching_constraints.clear();

    m_current_trust_factor_index = 0;

    m_n_binary_first_stage_decisions = 0;
    for (const auto& var : upper_level_vars()) {
        if (master_problem.get_var_type(var) == Binary) {
            ++m_n_binary_first_stage_decisions;
        }
    }

}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::analyze_current_master_problem_solution() {

    const auto &solution = this->current_master_solution();
    const auto status = solution.status();

    if (status == Unbounded) {
        throw Exception("Not implemented: cannot handle unbounded master problem, yet.");
    }

    if (status == Infeasible) {

        if ((!m_stability_center || current_radius() >= m_n_binary_first_stage_decisions)) {
            set_reason(Proved);
            terminate();
            return;
        } else {
            add_reversed_local_branching_constraint();
            update_radius();
            update_local_branching_constraint();
            return;
        }

    }

    if (status != Optimal) {
        const auto reason = solution.reason();
        set_status(status);
        set_reason(reason);
        terminate();
        return;
    }

    if (!m_stability_center) {
        m_stability_center = current_master_solution();
        update_local_branching_constraint();
    }

    if (parent().get_status() != Feasible) {
        set_status(SubOptimal);
    }

}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::update_local_branching_constraint() {

    assert(m_stability_center.has_value());

    auto& master_problem = this->master_problem();

    const auto distance_to_point = build_distance_to_point(m_stability_center.value());

    if (!m_local_branching_constraint) {
        m_local_branching_constraint = master_problem.add_ctr(distance_to_point <= current_radius(), "__local_branching");
    } else {
        assert(master_problem.has(*m_local_branching_constraint));

        // The implementation of set_ctr_row is currently unsafe (2024/03/06)
        // In case of doubt, use the following two lines instead of the currently active

        //master_problem.remove(*m_local_branching_constraint);
        //m_local_branching_constraint = master_problem.add_ctr(distance_to_point <= m_current_radius);

        master_problem.set_ctr_row(*m_local_branching_constraint, Row(distance_to_point, current_radius()));
    }

}

idol::Expr<idol::Var, idol::Var>
idol::Robust::CCGStabilizers::TrustRegion::Strategy::build_distance_to_point(const idol::Solution::Primal &t_point) {

    const auto& master_problem = this->master_problem();

    Expr result;
    for (const auto& var : upper_level_vars()) {

        const auto type = master_problem.get_var_type(var);

        if (type != Binary) {
            continue;
        }

        const auto value = t_point.get(var);

        if (value > .5) {
            result += 1 - var;
        } else {
            result += var;
        }

    }

    return result;
}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::add_reversed_local_branching_constraint() {


    auto& master_problem = this->master_problem();

    const auto distance_to_point = build_distance_to_point(m_stability_center.value());
    const double current_radius = this->current_radius();

    auto c = master_problem.add_ctr(distance_to_point >= current_radius + 1);

    m_reversed_local_branching_constraints.emplace_back(c, m_stability_center.value(), current_radius);

}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::remove_all_stabilization_constraints(Model& t_model) {

    if (m_local_branching_constraint && t_model.has(*m_local_branching_constraint)) {
        t_model.remove(*m_local_branching_constraint);
    }

    for (const auto& [ctr, point, radius] : m_reversed_local_branching_constraints) {
        if (t_model.has(ctr)) {
            t_model.remove(ctr);
        }
    }

}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::analyze_last_separation_solution() {

    const auto& parent = this->parent();
    const auto& current_separation_solution = this->current_separation_solution();
    const auto status = current_separation_solution.status();

    if (status != Optimal) {
        set_status(status);
        set_reason(current_separation_solution.reason());
        terminate();
        return;
    }

    // TODO for <= only
    if (current_separation_solution.objective_value() > Tolerance::Feasibility) {
        return;
    }

    set_status(Feasible);
    set_reason(Proved);

    // Update UB
    const auto& current_master_solution = this->current_master_solution();
    const double iter_obj = current_master_solution.objective_value();
    const double ub = parent.get_best_obj();
    if (iter_obj <= ub) {
        set_best_obj(iter_obj);
        set_incumbent(current_master_solution);
    }

    // Update LB
    auto lb_model = this->master_problem().copy();
    remove_all_stabilization_constraints(lb_model);
    lb_model.optimize();
    const double lb = parent.get_best_bound();
    const double iter_lb = lb_model.get_best_obj();
    if (iter_lb >= lb) {
        set_best_bound(iter_lb);
    }

    // Check if we closed the gap
    check_stopping_condition();

    if (parent.is_terminated()) {
        return;
    }

    // Update stability center
    add_reversed_local_branching_constraint();
    m_stability_center = current_master_solution;
    update_local_branching_constraint();

}

unsigned int idol::Robust::CCGStabilizers::TrustRegion::Strategy::current_radius() const {

    if (m_current_trust_factor_index >= m_trust_factor.size()) {
        return m_n_binary_first_stage_decisions;
    }

    return std::ceil( m_trust_factor[m_current_trust_factor_index] * (double) m_n_binary_first_stage_decisions);
}

void idol::Robust::CCGStabilizers::TrustRegion::Strategy::update_radius() {

    double old_radius = current_radius();

    do {
        ++m_current_trust_factor_index;
    } while (m_current_trust_factor_index < m_trust_factor.size() && current_radius() == old_radius);

}
