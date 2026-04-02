//
// Created by Henri on 02/04/2026.
//
#include "idol/mixed-integer/optimizers/presolve/VariableFixing.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

bool idol::Presolvers::VariableFixing::execute(Model& t_model) {

    const auto& env = t_model.env();
    const double tol_feasibility = env.get_tol_feasibility();

    std::list<Var> to_be_fixed;
    for (const auto& var : t_model.vars()) {
        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);
        if (equals(lb, ub, tol_feasibility)) {
            to_be_fixed.push_back(var);
        }
    }

    for (const auto& var : to_be_fixed) {

        const auto& column = t_model.get_var_column(var);
        const double value = t_model.get_var_ub(var);

        for (const auto& [ctr, coefficient] : column) {
            const double rhs = t_model.get_ctr_rhs(ctr);
            t_model.set_ctr_rhs(ctr, rhs - coefficient * value);
        }

        t_model.remove(var);

    }

    return !to_be_fixed.empty();
}

void idol::Presolvers::VariableFixing::log_after_termination() const {
    AbstractPresolver::log_after_termination();
    std::cout << "TODO" << std::endl;
}

idol::Presolvers::AbstractPresolver* idol::Presolvers::VariableFixing::clone() const {
    return new VariableFixing();
}
