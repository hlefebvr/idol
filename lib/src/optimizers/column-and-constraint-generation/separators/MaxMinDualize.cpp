//
// Created by henri on 08.02.24.
//
#include "idol/optimizers/column-and-constraint-generation/separators/MaxMinDualize.h"
#include "idol/modeling/solutions/Solution.h"
#include "idol/modeling/models/Model.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/optimizers/wrappers/MibS/MibS.h"
#include "idol/modeling/models/dualize.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"

idol::ColumnAndConstraintGenerationSeparator *idol::ColumnAndConstraintGenerationSeparators::MaxMinDualize::clone() const {
    return new MaxMinDualize(*this);
}

idol::Solution::Primal idol::ColumnAndConstraintGenerationSeparators::MaxMinDualize::operator()(
        const idol::Optimizers::ColumnAndConstraintGeneration &t_parent,
        const Solution::Primal &t_upper_level_solution,
        const Row& t_row,
        CtrType t_type) const {

    std::cout << "CURRENT MASTER SOLUTION "
              << "\n************************\n"
              << t_upper_level_solution
              << "\n************************"
              << std::endl;


    const auto& uncertainty_set = t_parent.uncertainty_set();

    Model primal(uncertainty_set.env());

    const auto& original_model = t_parent.parent();
    const auto& lower_level_variables = t_parent.lower_level_variables();

    for (const auto& var : original_model.vars()) {

        if (var.get(lower_level_variables) == MasterId) {
            continue;
        }

        const double lb = original_model.get_var_lb(var);
        const double ub = original_model.get_var_ub(var);
        const auto type = original_model.get_var_type(var);

        primal.add(var, TempVar(lb, ub, type, Column()));

    }


    const auto& lower_level_constraints = t_parent.lower_level_constraints();

    for (const auto& ctr : original_model.ctrs()) {

        if (ctr.get(lower_level_constraints) == MasterId) {
            continue;
        }

        add_lower_level_constraint(primal, t_parent, t_upper_level_solution, ctr);

    }

    set_lower_level_objective(primal, t_parent, t_upper_level_solution);

    auto dual = dualize(primal, true);

    // add uncertain parameters to primal

    for (const auto& var : uncertainty_set.vars()) {

        const auto lb = uncertainty_set.get_var_lb(var);
        const auto ub = uncertainty_set.get_var_ub(var);
        const auto type = uncertainty_set.get_var_type(var);

        if (!dual.has(var)) {
            dual.add(var, TempVar(lb, ub, type, Column()));
            continue;
        }

        dual.set_var_lb(var, lb);
        dual.set_var_ub(var, ub);
        dual.set_var_type(var, type);

    }

    for (const auto& ctr : uncertainty_set.ctrs()) {

        const auto& row = uncertainty_set.get_ctr_row(ctr);
        const auto type = uncertainty_set.get_ctr_type(ctr);

        dual.add(ctr, TempCtr(Row(row), type));

    }

    dual.use(Gurobi().with_logs(true));

    dual.set_obj_sense(Minimize);
    dual.set_obj_expr(-1. * dual.get_obj_expr());

    std::cout << "PRIMAL OF SECOND-STAGE "
              << "\n************************\n"
              << primal
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

    return save_primal(uncertainty_set, dual);
}

void idol::ColumnAndConstraintGenerationSeparators::MaxMinDualize::add_lower_level_constraint(idol::Model &t_primal,
                                                                                              const idol::Optimizers::ColumnAndConstraintGeneration &t_parent,
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
idol::ColumnAndConstraintGenerationSeparators::MaxMinDualize::fix(const idol::LinExpr<idol::Var> &t_expr,
                                                                  const idol::Optimizers::ColumnAndConstraintGeneration &t_parent,
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

void idol::ColumnAndConstraintGenerationSeparators::MaxMinDualize::set_lower_level_objective(idol::Model &t_primal,
                                                                                             const idol::Optimizers::ColumnAndConstraintGeneration &t_parent,
                                                                                             const idol::Solution::Primal &t_upper_level_solution) const {

    const auto& objective = t_parent.parent().get_obj_expr().linear();

    auto primal_objective = fix(objective, t_parent, t_upper_level_solution);

    t_primal.set_obj_expr(std::move(primal_objective));

}
