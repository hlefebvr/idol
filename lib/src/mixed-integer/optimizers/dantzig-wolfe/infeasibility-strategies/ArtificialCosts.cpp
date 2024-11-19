//
// Created by henri on 01.11.23.
//
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::DantzigWolfe::ArtificialCosts::Strategy::Strategy(double t_initial_costs,
                                                        double t_update_factor,
                                                        unsigned int t_max_updates_before_phase_I)
    : m_initial_costs(t_initial_costs),
      m_update_factor(t_update_factor),
      m_max_updates_before_phase_I(t_max_updates_before_phase_I) {

}


void idol::DantzigWolfe::ArtificialCosts::Strategy::execute(Optimizers::DantzigWolfeDecomposition &t_parent) {

    auto& formulation = t_parent.formulation();

    save_objective_function(t_parent.parent());
    create_artificial_variables(formulation);

    Optimizers::DantzigWolfeDecomposition::ColumnGeneration column_generation(t_parent, false, Inf);

    find_initial_columns(column_generation);

    restore_objective_function(formulation);
    delete_artificial_variables(formulation);

    if (this->status() != Feasible) {
        set_best_obj(Inf);
        set_best_bound(-Inf);
        return;
    }

    column_generation.set_best_bound_stop(t_parent.get_param_best_bound_stop());
    column_generation.execute();

    const auto status = column_generation.status();

    set_status(status);
    set_reason(column_generation.reason());
    set_best_obj(column_generation.best_obj());
    set_best_bound(column_generation.best_bound());

    if (status == Optimal || status == Feasible) {
        set_primal_solution(column_generation.primal_solution());
    }

}

void idol::DantzigWolfe::ArtificialCosts::Strategy::find_initial_columns(idol::Optimizers::DantzigWolfeDecomposition::ColumnGeneration &t_column_generation) {

    double current_costs = m_initial_costs;
    auto& formulation = t_column_generation.parent().formulation();

    for (unsigned int current_phase = 0 ; current_phase < m_max_updates_before_phase_I ; ++current_phase) {

        t_column_generation.execute();

        const auto status = t_column_generation.status();

        if (status == Fail || status == Unbounded || status == InfOrUnbnd) {
            break;
        }

        if (status != Optimal) {
            set_status(status);
            set_reason(t_column_generation.reason());
            return;
        }

        const auto& primal_values = t_column_generation.primal_solution();

        if (all_artificial_variables_are_non_basic(primal_values)) {
            set_status(Feasible);
            return;
        }

        update_objective_function(formulation, primal_values, true);

        current_costs *= m_update_factor;
    }

    update_objective_function(formulation, PrimalPoint(), false);

    t_column_generation.execute();

    const auto status = t_column_generation.status();

    if (status != Optimal) {
        set_status(status);
        set_reason(t_column_generation.reason());
        return;
    }

    const auto& primal_values = t_column_generation.primal_solution();

    if (!all_artificial_variables_are_non_basic(primal_values)) {
        set_status(Infeasible);
        set_reason(Proved);
        return;
    }

    set_status(Feasible);

}

void idol::DantzigWolfe::ArtificialCosts::Strategy::save_objective_function(const Model& t_original_formulation) {

    m_objective_function = t_original_formulation.get_obj_expr();

}

void idol::DantzigWolfe::ArtificialCosts::Strategy::create_artificial_variables(
        idol::DantzigWolfe::Formulation &t_formulation) {

    auto& master = t_formulation.master();
    const auto add_artificial_variable = [&](const Ctr& t_ctr, double t_coeff) {
        LinExpr<Ctr> column;
        column.set(t_ctr, t_coeff);
        auto var = master.add_var(0., Inf, Continuous, m_initial_costs, std::move(column));
        m_artificial_variables.emplace_back(var);
        return var;
    };

    for (const auto& ctr : master.ctrs()) {
        switch (master.get_ctr_type(ctr)) {
            case LessOrEqual:
                add_artificial_variable(ctr, -1.);
                break;
            case GreaterOrEqual:
                add_artificial_variable(ctr, 1.);
                break;
            case Equal:
                add_artificial_variable(ctr, -1.);
                add_artificial_variable(ctr, 1.);
                break;
        }
    }

}

void idol::DantzigWolfe::ArtificialCosts::Strategy::delete_artificial_variables(
        idol::DantzigWolfe::Formulation &t_formulation) {

    auto& master = t_formulation.master();
    for (const auto& var : m_artificial_variables) {
        master.remove(var);
    }

    m_artificial_variables.clear();

}

bool idol::DantzigWolfe::ArtificialCosts::Strategy::all_artificial_variables_are_non_basic(const PrimalPoint &t_primal_values) const {

    for (const auto& var : m_artificial_variables) {
        if (!equals(t_primal_values.get(var), 0., Tolerance::Feasibility)) {
            return false;
        }
    }

    return true;
}

void idol::DantzigWolfe::ArtificialCosts::Strategy::restore_objective_function(DantzigWolfe::Formulation& t_formulation) {
    t_formulation.update_obj(m_objective_function);
}

void idol::DantzigWolfe::ArtificialCosts::Strategy::update_objective_function(
        DantzigWolfe::Formulation& t_formulation,
        const PrimalPoint& t_primal_values,
        bool t_include_original_objective_function) {

    if (!t_include_original_objective_function) {
        t_formulation.update_obj(idol_Sum(var, m_artificial_variables, var));
        return;
    }

    auto& master = t_formulation.master();
    AffExpr<Var> objective = m_objective_function;

    for (const auto& var : m_artificial_variables) {
        if (!equals(t_primal_values.get(var), 0., Tolerance::Feasibility)) {
            const double current_cost = master.get_var_obj(var);
            master.set_var_obj(var, m_update_factor * current_cost);
        }
    }

    t_formulation.update_obj(objective);

}

idol::DantzigWolfe::ArtificialCosts &
idol::DantzigWolfe::ArtificialCosts::with_max_updates_before_phase_I(unsigned int t_value) {

    if (m_max_updates_before_phase_I.has_value()) {
        throw Exception("Maximum updates before phase I has already been configured.");
    }

    m_max_updates_before_phase_I = t_value;

    return *this;
}

idol::DantzigWolfe::ArtificialCosts &idol::DantzigWolfe::ArtificialCosts::with_update_factor(double t_value) {

    if (m_update_factor.has_value()) {
        throw Exception("Update factor has already been configured.");
    }

    m_update_factor = t_value;

    return *this;
}

idol::DantzigWolfe::ArtificialCosts &idol::DantzigWolfe::ArtificialCosts::with_initial_costs(double t_value) {

    if (m_initial_costs.has_value()) {
        throw Exception("Initial costs have already been configured.");
    }

    m_initial_costs = t_value;

    return *this;
}
