//
// Created by henri on 06.03.24.
//
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/NoStabilization.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"

idol::Robust::CCGStabilizer::Strategy *
idol::Robust::CCGStabilizers::NoStabilization::operator()() const {
    return new Strategy();
}

idol::Robust::CCGStabilizer *idol::Robust::CCGStabilizers::NoStabilization::clone() const {
    return new NoStabilization(*this);
}

void idol::Robust::CCGStabilizers::NoStabilization::Strategy::initialize() {

}

void idol::Robust::CCGStabilizers::NoStabilization::Strategy::analyze_current_master_problem_solution() {

    const auto& solution = this->current_master_solution();
    const auto status = solution.status();

    if (status == Unbounded) {
        throw Exception("Not implemented: cannot handle unbounded master problem, yet.");
    }

    if (status == Infeasible) {
        set_status(Infeasible);
        set_reason(Proved);
        terminate();
        return;
    }

    if (status != Optimal) {
        const auto reason = solution.reason();
        set_status(status);
        set_reason(reason);
        terminate();
        return;
    }

    const double objective = solution.objective_value();

    set_best_bound(objective);
    set_status(SubOptimal);

}

void idol::Robust::CCGStabilizers::NoStabilization::Strategy::analyze_last_separation_solution() {

    const auto& last_scenario = this->current_separation_solution();
    const auto status = last_scenario.status();

    if (status != Optimal) {
        set_status(status);
        set_reason(last_scenario.reason());
        terminate();
        return;
    }

    // TODO for <= only
    if (last_scenario.objective_value() <= Tolerance::Feasibility) {
        set_status(Optimal);
        set_reason(Proved);
        set_best_obj(parent().get_best_bound());
        terminate();
        return;
    }

}
