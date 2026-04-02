//
// Created by Henri on 02/04/2026.
//
#include "idol/mixed-integer/optimizers/presolve/BoundRounding.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

bool idol::Presolvers::BoundRounding::execute(Model& t_model) {

    const auto& env = t_model.env();
    const double tol_integer = env.get_tol_integer();
    const double tol_feasibility = env.get_tol_feasibility();

    unsigned int changes = 0;

    for (const auto& var : t_model.vars()) {

        const auto type = t_model.get_var_type(var);
        const double lb = t_model.get_var_lb(var);
        const double ub = t_model.get_var_ub(var);

        if (type != Continuous) {

            double new_lb = std::ceil(lb - tol_integer);
            double new_ub = std::floor(ub + tol_integer);

            if (type == Binary) {
                new_lb = std::max(0., new_lb);
                new_ub = std::min(1., new_ub);
            }

            if (!equals(lb, new_lb, tol_feasibility)) {
                t_model.set_var_lb(var, new_lb);
                changes++;
            }

            if (!equals(ub, new_ub, tol_feasibility)) {
                t_model.set_var_ub(var, new_ub);
                changes++;
            }

        }
    }

    m_n_total_changes += changes;

    return changes > 0;
}

void idol::Presolvers::BoundRounding::log_after_termination() const {
    AbstractPresolver::log_after_termination();

    std::cout << "BoundRounding: " << m_n_total_changes;
}

idol::Presolvers::AbstractPresolver* idol::Presolvers::BoundRounding::clone() const {
    return new BoundRounding(*this);
}
