//
// Created by henri on 30.10.23.
//
#include <utility>

#include "idol/optimizers/wrappers/MinKnap/Optimizers_MinKnap.h"

#ifdef IDOL_USE_MINKNAP

extern "C" {
#include "minknap.h"
}

idol::Optimizers::MinKnap::MinKnap(const idol::Model &t_model) : OptimizerWithLazyUpdates(t_model) {

}

void idol::Optimizers::MinKnap::hook_build() {}

void idol::Optimizers::MinKnap::hook_write(const std::string &t_name) {
    throw Exception("Writing to file using MinKnap is not available.");
}

std::optional<double> idol::Optimizers::MinKnap::hook_add(const idol::Var &t_var, bool t_add_column) {
    return {};
}

bool idol::Optimizers::MinKnap::hook_add(const idol::Ctr &t_ctr) {

    const auto& model = parent();

    if (model.ctrs().size() > 1) {
        throw Exception("Cannot add more than one constraint to MinKnap.");
    }

    if (model.get_ctr_type(t_ctr) != LessOrEqual) {
        throw Exception("MinKnap only handles <= constraints.");
    }

    return true;
}

void idol::Optimizers::MinKnap::set_solution_index(unsigned int t_index) {
    if (t_index > 0) {
        throw Exception("Solution index out of range");
    }
}

double idol::Optimizers::MinKnap::get_var_ray(const idol::Var &t_var) const {
    throw Exception("Not available.");
}

double idol::Optimizers::MinKnap::get_ctr_dual(const idol::Ctr &t_ctr) const {
    throw Exception("Not available.");
}

double idol::Optimizers::MinKnap::get_ctr_farkas(const idol::Ctr &t_ctr) const {
    throw Exception("Not available.");
}

void idol::Optimizers::MinKnap::hook_optimize() {

    const auto& model = parent();
    const double sense_factor = model.get_obj_sense() == Minimize ? -1. : 1.;
    const auto& ctr = model.get_ctr_by_index(0);
    const auto& row = model.get_ctr_row(ctr);

    struct Item {
        const Var variable;
        const double profit;
        const double weight;
        Item(Var t_var, double t_profit, double t_weight) : variable(std::move(t_var)), profit(t_profit), weight(t_weight) {}
    };

    double unscaled_capacity = as_numeric(row.rhs());
    double fixed_profit = as_numeric(model.get_obj_expr().constant());
    std::list<Item> free_variables;

    // "presolve"
    for (const auto& var : model.vars()) {

        const auto type = model.get_var_type(var);

        if (type == Continuous) {
            throw Exception("Cannot handle continuous variables.");
        }

        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);

        if (lb < -Tolerance::Integer || ub > 1 + Tolerance::Integer) {
            throw Exception("Cannot handle general integer variables.");
        }

        const auto& column = model.get_var_column(var);
        const double weight = as_numeric(column.linear().get(ctr));
        const double profit = as_numeric(column.obj());

        if (ub < 1. - 2 * Tolerance::Integer) {
            lazy(var).impl() = 0.; // fixed to 0
            continue;
        }

        if (lb > 2 * Tolerance::Integer) {
            lazy(var).impl() = 1.; // fixed to 1
            unscaled_capacity -= weight;
            fixed_profit += profit;
            continue;
        }

        free_variables.emplace_back(var, sense_factor * profit, weight);

    }

    // prepare to solve
    const unsigned int n = free_variables.size();
    auto* profits = new int[n];
    auto* weights = new int[n];
    auto* values = new int[n];
    auto scaling_factor = (double) 1000;
    auto capacity = (int) (scaling_factor * unscaled_capacity);

    unsigned int i = 0;
    for (const auto& [var, profit, weight] : free_variables) {
        profits[i] = (int) (scaling_factor * profit);
        weights[i] = (int) (scaling_factor * weight);
        ++i;
    }

    // solve
    m_objective_value = (double) minknap((int) n, profits, weights, values, capacity) / scaling_factor;
    m_objective_value.value() *= sense_factor;
    m_objective_value.value() += fixed_profit;

    // save solution
    m_solution_status = Optimal;
    m_solution_reason = Proved;
    i = 0;
    for (const auto& [var, profit, weight] : free_variables) {
        lazy(var).impl() = values[i];
        ++i;
    }

    // free memory
    delete[] profits;
    delete[] weights;
    delete[] values;
}

idol::SolutionStatus idol::Optimizers::MinKnap::get_status() const {
    return m_solution_status;
}

idol::SolutionReason idol::Optimizers::MinKnap::get_reason() const {
    return m_solution_reason;
}

double idol::Optimizers::MinKnap::get_best_obj() const {
    return m_objective_value.value();
}

double idol::Optimizers::MinKnap::get_best_bound() const {
    return get_best_obj();
}

double idol::Optimizers::MinKnap::get_var_primal(const idol::Var &t_var) const {
    if (!lazy(t_var).has_impl()) {
        throw Exception("Not available.");
    }
    return lazy(t_var).impl().value();
}

double idol::Optimizers::MinKnap::get_relative_gap() const {
    return m_objective_value.has_value() ? 0. : +Inf;
}

double idol::Optimizers::MinKnap::get_absolute_gap() const {
    return get_relative_gap();
}

unsigned int idol::Optimizers::MinKnap::get_n_solutions() const {
    return m_objective_value.has_value() ? 1 : 0;
}

#endif
