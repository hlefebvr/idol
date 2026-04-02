//
// Created by Henri on 02/04/2026.
//
#include "idol/mixed-integer/optimizers/presolve/OneRowBoundTightening.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

unsigned int idol::Presolvers::OneRowBoundTightening::do_single_row_bound_tightening(Model& t_model, const LinExpr<Var>& t_row, CtrType t_type, double t_rhs) {

    unsigned int result = 0;

    const auto& env = t_model.env();
    const double tol_feasibility = env.get_tol_feasibility();

    const double min_activity = get_min_activity(t_model, t_row);

    if (is_neg_inf(min_activity)) {
        return 0;
    }

    for (const auto& [var, coefficient] : t_row) {

        if (coefficient > 0) {

            const double lb = t_model.get_var_lb(var);
            const double ub = t_model.get_var_ub(var);
            const double new_ub = (t_rhs - (min_activity - coefficient * lb)) / coefficient;

            if (ub - new_ub > 1e3 * tol_feasibility && std::abs(new_ub) < 1e8) {
                t_model.set_var_ub(var, new_ub);
                result++;
            }

        } else {

            const double lb = t_model.get_var_lb(var);
            const double ub = t_model.get_var_ub(var);
            const double new_lb = (t_rhs - (min_activity - coefficient * ub)) / coefficient;

            if (new_lb -lb > 1e3 * tol_feasibility && std::abs(new_lb) < 1e8) {
                t_model.set_var_lb(var, new_lb);
                result++;
            }


        }

    }

    return result;
}

bool idol::Presolvers::OneRowBoundTightening::execute(Model& t_model) {

    unsigned int changes = 0;

    for (const auto& ctr : t_model.ctrs()) {

        const auto type = t_model.get_ctr_type(ctr);
        const auto& row = t_model.get_ctr_row(ctr);
        const auto rhs = t_model.get_ctr_rhs(ctr);

        if (type == Equal) {
            changes += do_single_row_bound_tightening(t_model, row, LessOrEqual, rhs);
            changes += do_single_row_bound_tightening(t_model, row, GreaterOrEqual, rhs);
        } else {
            changes += do_single_row_bound_tightening(t_model, row, type, rhs);
        }

    }

    m_n_total_changes += changes;

    return changes > 0;

}

void idol::Presolvers::OneRowBoundTightening::log_after_termination() const {
    AbstractPresolver::log_after_termination();

    std::cout << "OneRowBoundTightening: " << m_n_total_changes;

}

idol::Presolvers::AbstractPresolver* idol::Presolvers::OneRowBoundTightening::clone() const {
    return new OneRowBoundTightening(*this);
}
