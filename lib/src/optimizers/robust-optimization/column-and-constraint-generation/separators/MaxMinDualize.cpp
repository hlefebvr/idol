//
// Created by henri on 08.02.24.
//
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/MaxMinDualize.h"
#include "idol/modeling/solutions/Solution.h"
#include "idol/modeling/models/Model.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/modeling/models/dualize.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Gurobi/Gurobi.h"

idol::Robust::ColumnAndConstraintGenerationSeparator *idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::clone() const {
    return new MaxMinDualize(*this);
}

idol::Solution::Primal idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::operator()(
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
        const Solution::Primal &t_upper_level_solution,
        const Row& t_row,
        CtrType t_type) const {

    /*
    std::cout << "CURRENT MASTER SOLUTION "
              << "\n************************\n"
              << t_upper_level_solution
              << "\n************************"
              << std::endl;
    */

    const auto& uncertainty_set = t_parent.uncertainty_set();
    auto& env = uncertainty_set.env();

    if (!t_parent.complete_recourse()) {

        Model primal = create_second_stage_primal_problem(env, t_parent, t_upper_level_solution);
        make_feasibility_problem(primal);

        const auto solution = solve_max_min(uncertainty_set, primal);

        std::cout << solution << std::endl;

        if (solution.status() != Optimal || solution.objective_value() < -Tolerance::Feasibility) {
            return solution;
        }

    }

    Model primal = create_second_stage_primal_problem(env, t_parent, t_upper_level_solution);

    throw Exception("STOP, Complete recourse not implemented");
}

void idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::add_lower_level_constraint(idol::Model &t_primal,
                                                                                            const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                                                                            const idol::Solution::Primal &t_upper_level_solution,
                                                                                            const idol::Ctr &t_ctr) const {

    const auto& model = t_parent.parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto& type = model.get_ctr_type(t_ctr);

    Expr<Var, Var> lhs = fix(row.linear(), t_parent, t_upper_level_solution);
    Constant rhs = row.rhs();

    if (!row.quadratic().empty()) {
        throw Exception("Quadratic terms in the lower level are not implemented.");
    }

    t_primal.add(t_ctr, TempCtr(Row(std::move(lhs), std::move(rhs)), type));

}

idol::Expr<idol::Var, idol::Var>
idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::fix(const idol::LinExpr<idol::Var> &t_expr,
                                                                const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                                                const idol::Solution::Primal &t_upper_level_solution) {
    Expr<Var, Var> result;

    const auto& lower_level_variables = t_parent.lower_level_variables();

    for (const auto& [var, constant] : t_expr) {

        const bool is_upper_level_variable = var.get(lower_level_variables) == MasterId;

        for (const auto& [param, coefficient] : constant.linear()) {

            if (is_upper_level_variable) {
                result += coefficient * param * t_upper_level_solution.get(var);
                continue;
            }

            result += coefficient * param * var;
        }

        if (constant.quadratic().size() > 0) {
            throw Exception("Quadratic terms in uncertainty not implemented.");
        }

        if (is_upper_level_variable) {
            result += constant.numerical() * t_upper_level_solution.get(var);
            continue;
        }

        result += constant.numerical() * var;

    }

    return result;
}

void idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::set_lower_level_objective(idol::Model &t_primal,
                                                                                           const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                                                                           const idol::Solution::Primal &t_upper_level_solution) const {

    const auto& objective = t_parent.parent().get_obj_expr().linear();

    auto primal_objective = fix(objective, t_parent, t_upper_level_solution);

    t_primal.set_obj_expr(std::move(primal_objective));

}

idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize &
idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::with_optimizer(
        const idol::OptimizerFactory &t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("An optimizer for MaxMinDualize has already been given.");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::MaxMinDualize(const idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize &t_src)
        : m_optimizer_factory(t_src.m_optimizer_factory->clone()) {

}

idol::Model
idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::create_second_stage_primal_problem(idol::Env &t_env,
                                                                                               const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                                                                               const idol::Solution::Primal &t_upper_level_solution) const {

    Model result(t_env);

    const auto& original_model = t_parent.parent();
    const auto& lower_level_variables = t_parent.lower_level_variables();

    for (const auto& var : original_model.vars()) {

        if (var.get(lower_level_variables) == MasterId) {
            continue;
        }

        const double lb = original_model.get_var_lb(var);
        const double ub = original_model.get_var_ub(var);
        const auto type = original_model.get_var_type(var);

        result.add(var, TempVar(lb, ub, type, Column()));

    }

    const auto& lower_level_constraints = t_parent.lower_level_constraints();

    for (const auto& ctr : original_model.ctrs()) {

        if (ctr.get(lower_level_constraints) == MasterId) {
            continue;
        }

        add_lower_level_constraint(result, t_parent, t_upper_level_solution, ctr);

    }

    set_lower_level_objective(result, t_parent, t_upper_level_solution);

    return std::move(result);
}

void
idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::make_feasibility_problem(idol::Model &t_model) const {

    t_model.set_obj_expr(Expr());

    const auto add_artificial_variable = [&](const Ctr& t_ctr, double t_value) {

        Column column(1);
        column.linear().set(t_ctr, t_value);

        t_model.add_var(0, Inf, Continuous, column, std::string("art_") + t_ctr.name());


    };

    for (const auto& ctr : t_model.ctrs()) {

        switch (t_model.get_ctr_type(ctr)) {
            case LessOrEqual:
                add_artificial_variable(ctr, -1);
                break;
            case GreaterOrEqual:
                add_artificial_variable(ctr, 1);
                break;
            case Equal:
                add_artificial_variable(ctr, -1);
                add_artificial_variable(ctr, 1);
                break;
            default: throw Exception("Enum out of bounds.");
        }

    }


}

idol::Solution::Primal
idol::Robust::ColumnAndConstraintSeparators::MaxMinDualize::solve_max_min(const idol::Model &t_max,
                                                                          const idol::Model &t_min) const {

    auto dual = dualize(t_min, true);

    // add uncertain parameters to primal

    for (const auto& var : t_max.vars()) {

        const auto lb = t_max.get_var_lb(var);
        const auto ub = t_max.get_var_ub(var);
        const auto type = t_max.get_var_type(var);

        if (!dual.has(var)) {
            dual.add(var, TempVar(lb, ub, type, Column()));
            continue;
        }

        dual.set_var_lb(var, lb);
        dual.set_var_ub(var, ub);
        dual.set_var_type(var, type);

    }

    for (const auto& ctr : t_max.ctrs()) {

        const auto& row = t_max.get_ctr_row(ctr);
        const auto type = t_max.get_ctr_type(ctr);

        dual.add(ctr, TempCtr(Row(row), type));

    }

    dual.set_obj_sense(Minimize);
    dual.set_obj_expr(-1. * dual.get_obj_expr());

    if (!m_optimizer_factory) {
        throw Exception("An optimizer for MaxMinDualize has not been given.");
    }

    dual.use(*m_optimizer_factory);

    std::cout << "PRIMAL OF SECOND-STAGE "
              << "\n************************\n"
              << t_min
              << "\n************************"
              << std::endl;

    std::cout << "SEPARATION PROBLEM"
              << "\n************************\n"
              << dual
              << "\n************************"
              << std::endl;

    dual.optimize();

    const auto status = dual.get_status();

    if (status != Optimal) {
        Solution::Primal result;
        result.set_status(status);
        result.set_reason(dual.get_reason());
        return result;
    }

    return save_primal(t_max, dual);
}
