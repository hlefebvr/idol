//
// Created by henri on 08.02.24.
//
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Bilevel.h"
#include "idol/modeling/solutions/Solution.h"
#include "idol/modeling/models/Model.h"
#include "idol/modeling/objects/Versions.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"

idol::Robust::CCGSeparator *idol::Robust::CCGSeparators::Bilevel::clone() const {
    return new Bilevel(*this);
}

idol::Solution::Primal idol::Robust::CCGSeparators::Bilevel::operator()(
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
        const Solution::Primal &t_upper_level_solution,
        const Row& t_row,
        CtrType t_type) const {

    Model model = t_parent.uncertainty_set().copy();

    // Add lower level variables and constraints (fixes upper level variables and add parameters as variables)
    add_lower_level_variables(model, t_parent);
    add_lower_level_constraints(model, t_parent, t_upper_level_solution);

    // Add lower level objective
    auto lower_level_objective = set_upper_and_lower_objectives(model, t_parent, t_upper_level_solution, t_row, t_type);

    std::cout << "First-Stage Decision \n" << t_upper_level_solution << std::endl;

    // Create MibS
    const auto& lower_level_variables = t_parent.lower_level_variables();
    const auto& lower_level_constraints = t_parent.lower_level_constraints();

    auto description = idol::Bilevel::Description(lower_level_variables,
                                                  lower_level_constraints,
                                                  lower_level_objective);

    auto optimizer = idol::Bilevel::MibS(description).with_logs(false);

    model.use(optimizer);

    // Solve
    model.optimize();

    const auto status = model.get_status();

    if (status != Optimal) {
        Solution::Primal result;
        result.set_status(status);
        result.set_reason(model.get_reason());
        return result;
    }

    auto result = save_primal(model);

    result.set_objective_value(-result.objective_value());

    std::cout << result << std::endl;

    return result;
}

void idol::Robust::CCGSeparators::Bilevel::add_lower_level_variables(
        idol::Model &t_hpr,
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent) const {

    const auto& model = t_parent.parent();
    const auto& lower_level_variables = t_parent.lower_level_variables();

    for (const auto& var : model.vars()) {

        if (var.get(lower_level_variables) == MasterId) {
            continue;
        }

        const double lb = model.get_var_lb(var);
        const double ub = model.get_var_ub(var);
        const auto type = model.get_var_type(var);

        t_hpr.add(var, TempVar(lb, ub, type, Column()));

    }

}


void idol::Robust::CCGSeparators::Bilevel::add_lower_level_constraints(
        idol::Model &t_hpr,
        const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
        const Solution::Primal &t_upper_level_solution) const {

    const auto& model = t_parent.parent();
    const auto& lower_level_constraints = t_parent.lower_level_constraints();

    for (const auto& ctr : model.ctrs()) {

        if (ctr.get(lower_level_constraints) == MasterId) {
            continue;
        }

        add_lower_level_constraint(t_hpr, t_parent, t_upper_level_solution, ctr);

    }

}

void idol::Robust::CCGSeparators::Bilevel::add_lower_level_constraint(idol::Model &t_hpr,
                                                                      const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                                                      const Solution::Primal &t_upper_level_solution,
                                                                      const idol::Ctr &t_ctr) const {

    const auto& model = t_parent.parent();
    const auto& row = model.get_ctr_row(t_ctr);
    const auto& type = model.get_ctr_type(t_ctr);

    Expr<Var, Var> lhs = fix_and_revert(row.linear(), t_parent, t_upper_level_solution);
    Expr<Var, Var> rhs = revert(row.rhs(), t_parent);

    if (!row.quadratic().empty()) {
        throw Exception("Quadratic terms in the lower level are not implemented.");
    }

    t_hpr.add(t_ctr, TempCtr(Row(std::move(lhs), std::move(rhs)), type));

}

idol::Expr<idol::Var, idol::Var> idol::Robust::CCGSeparators::Bilevel::set_upper_and_lower_objectives(
        idol::Model &t_hpr,
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
        const idol::Solution::Primal &t_upper_level_solution,
        const idol::Row &t_row,
        idol::CtrType t_type) const {

    if (t_type != GreaterOrEqual) {
        throw Exception("Only >= coupling constraints have been implemented.");
    }

    Expr<Var, Var> hpr_objective = fix_and_revert(t_row.linear(), t_parent, t_upper_level_solution) + revert(t_row.rhs(), t_parent);

    t_hpr.set_obj_sense(Minimize);
    t_hpr.set_obj_expr(std::move(hpr_objective));

    return -1. * hpr_objective;
}
