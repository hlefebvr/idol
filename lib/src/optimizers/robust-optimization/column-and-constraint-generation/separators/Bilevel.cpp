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

idol::Solution::Primal idol::Robust::CCGSeparators::Bilevel::solve_feasibility_separation_problem(
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
        const idol::Solution::Primal &t_upper_level_solution) const {

    std::cout << "WARNING: Introducing slack variables with fake upper bound of 1e4 to bound HPR for MibS." << std::endl;

    Model model = t_parent.uncertainty_set().copy();

    const auto& stage_description = t_parent.stage_description();
    const auto& lower_level_variables = stage_description.stage_vars();
    const auto& lower_level_constraints = stage_description.stage_ctrs();

    auto description = idol::Bilevel::LowerLevelDescription(lower_level_variables,
                                                            lower_level_constraints);


    // Add lower level variables and constraints (fixes upper level variables and add parameters as variables)
    add_lower_level_variables(model, t_parent);
    add_lower_level_constraints(model, t_parent, t_upper_level_solution);

    std::list<Var> slacks;

    const auto add_slack = [&](double t_coefficient, const Ctr& t_ctr) {
        Column column(-1);
        column.linear().set(t_ctr, t_coefficient);
        const auto slack = model.add_var(0, 1e4, Continuous, column);
        description.make_follower_var(slack);
        slacks.emplace_back(slack);
    };

    for (const auto& ctr : model.ctrs()) {

        if (description.is_leader(ctr)) {
            continue;
        }

        const auto type = model.get_ctr_type(ctr);
        switch (type) {
            case LessOrEqual:
                add_slack(-1, ctr);
                break;
            case GreaterOrEqual:
                add_slack(1, ctr);
                break;
            case Equal:
                add_slack(-1, ctr);
                add_slack(1, ctr);
                break;
        }

    }

    description.set_follower_obj_expr(idol_Sum(var, slacks, var));

    return solve_bilevel(t_parent, model, description);
}

idol::Solution::Primal idol::Robust::CCGSeparators::Bilevel::solve_separation_problem(
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

    // Create lower level description
    const auto& stage_description = t_parent.stage_description();
    const auto& lower_level_variables = stage_description.stage_vars();
    const auto& lower_level_constraints = stage_description.stage_ctrs();

    auto description = idol::Bilevel::LowerLevelDescription(lower_level_variables,
                                                            lower_level_constraints,
                                                            lower_level_objective);

    return solve_bilevel(t_parent, model, description);
}

idol::Solution::Primal idol::Robust::CCGSeparators::Bilevel::solve_bilevel(
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
        idol::Model& t_model,
        const idol::Bilevel::LowerLevelDescription &t_description) {

    auto optimizer = idol::Bilevel::MibS(t_description).with_logs(false);

    t_model.use(optimizer);

    // Solve
    t_model.optimize();

    const auto status = t_model.get_status();

    if (status != Optimal) {
        Solution::Primal result;
        result.set_status(status);
        result.set_reason(t_model.get_reason());
        return result;
    }

    auto result = save_primal(t_model);

    result.set_objective_value(-result.objective_value());

    return result;
}

void idol::Robust::CCGSeparators::Bilevel::add_lower_level_variables(
        idol::Model &t_hpr,
        const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent) const {

    const auto& model = t_parent.parent();
    const auto& stage_description = t_parent.stage_description();

    for (const auto& var : model.vars()) {

        if (stage_description.stage(var) == 1) {
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
    const auto& stage_description = t_parent.stage_description();

    for (const auto& ctr : model.ctrs()) {

        if (stage_description.stage(ctr) == 1) {
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
