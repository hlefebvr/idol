//
// Created by Henri on 02/04/2026.
//
#include "idol/mixed-integer/optimizers/presolve/AbstractPresolver.h"

#include "idol/mixed-integer/modeling/models/Model.h"

double idol::Presolvers::AbstractPresolver::get_min_activity(const Model& t_model, const LinExpr<Var>& t_expr) {

    double result = 0.;

    for (const auto& [var, coefficient] : t_expr) {

        if (coefficient > 0) {
            const double lb = t_model.get_var_lb(var);
            if (is_neg_inf(lb)) {
                return -Inf;
            }
            result += coefficient * lb;
        } else {
            const double ub = t_model.get_var_ub(var);
            if (is_pos_inf(ub)) {
                return -Inf;
            }
            result += coefficient * ub;
        }

    }

    return result;
}

double idol::Presolvers::AbstractPresolver::get_max_activity(const Model& t_model, const LinExpr<Var>& t_expr) {

    double result = 0.;

    for (const auto& [var, coefficient] : t_expr) {

        if (coefficient > 0) {
            const double ub = t_model.get_var_ub(var);
            if (is_pos_inf(ub)) {
                return Inf;
            }
            result += coefficient * ub;
        } else {
            const double lb = t_model.get_var_lb(var);
            if (is_neg_inf(lb)) {
                return Inf;
            }
            result += coefficient * lb;
        }

    }

    return result;

}
