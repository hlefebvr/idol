//
// Created by henri on 30.08.24.
//

#include "KKT.h"
#include "idol/modeling/expressions/operations/operators.h"

idol::Reformulators::KKT::KKT(const idol::Model &t_src_model,
                              const std::function<bool(const Var &)> &t_indicator_for_vars,
                              const std::function<bool(const Ctr &)> &t_indicator_for_ctrs)
                              : m_src_model(t_src_model),
                                m_indicator_for_vars(t_indicator_for_vars),
                                m_indicator_for_ctrs(t_indicator_for_ctrs)
{

    create_primal_variables();
    create_primal_constraints();

    create_dual_variables_for_constraints();
    // create_dual_variables_for_upper_bounds();
    // create_dual_variables_for_lower_bounds();
    // create_dual_constraints();

    // create_complementary_for_constraints();
    // create_complementary_for_upper_bounds();
    // create_complementary_for_lower_bounds();

}

void idol::Reformulators::KKT::create_primal_variables() {

    const unsigned int n_variables = m_src_model.vars().size();
    auto& env = m_src_model.env();
    m_primal_vars.resize(n_variables);

    for (const auto& var : m_src_model.vars()) {

        if (!m_indicator_for_vars(var)) {
            continue;
        }

        const unsigned int index = m_src_model.get_var_index(var);

        if (m_keep_primal_objects) {
            m_primal_vars[index] = var;
            continue;
        }

        m_primal_vars[index] = Var(env, 0, Inf, Continuous, "primal_" + var.name());

    }

}

void idol::Reformulators::KKT::create_primal_constraints() {

    const unsigned int n_constraints = m_src_model.ctrs().size();
    auto& env = m_src_model.env();
    m_primal_ctrs.resize(n_constraints);

    for (const auto& ctr : m_src_model.ctrs()) {

        if (!m_indicator_for_ctrs(ctr)) {
            continue;
        }

        if (m_keep_primal_objects) {
            m_primal_ctrs[m_src_model.get_ctr_index(ctr)] = ctr;
            continue;
        }

        const unsigned int index = m_src_model.get_ctr_index(ctr);
        m_primal_ctrs[index] = Ctr(env, TempCtr(), "primal_" + ctr.name());

    }

}

void idol::Reformulators::KKT::create_dual_variables_for_constraints() {

    const unsigned int n_constraints = m_src_model.ctrs().size();
    auto& env = m_src_model.env();

    m_dual_vars_for_constraints.resize(n_constraints);

    for (const auto& ctr : m_src_model.ctrs()) {

        if (!m_indicator_for_ctrs(ctr)) {
            continue;
        }

        const unsigned int index = m_src_model.get_ctr_index(ctr);
        m_dual_vars_for_constraints[index] = Var(env, 0, idol::Inf, idol::Continuous, "dual_" + ctr.name());

    }

}

const idol::Var &idol::Reformulators::KKT::to_destination_space(const idol::Var &t_src_var) const {
    return m_primal_vars[m_src_model.get_var_index(t_src_var)].value();
}

const idol::Var &idol::Reformulators::KKT::dual(const idol::Ctr &t_ctr) const {
    return m_dual_vars_for_constraints[m_src_model.get_ctr_index(t_ctr)].value();
}

void idol::Reformulators::KKT::add_dual_variables(Model& t_destination) {

    add_variables(t_destination, m_dual_vars_for_constraints);
    add_variables(t_destination, m_dual_vars_for_upper_bounds);
    add_variables(t_destination, m_dual_vars_for_lower_bounds);

}

void idol::Reformulators::KKT::add_primal_variables(idol::Model &t_destination) {

    for (const auto& opt_var : m_primal_vars) {

        if (!opt_var.has_value()) {
            continue;
        }

        const auto& var = opt_var.value();
        const double lb = m_src_model.get_var_lb(var);
        const double ub = m_src_model.get_var_ub(var);
        const auto type = m_src_model.get_var_type(var);

        t_destination.add(var, TempVar(lb, ub, type, Column()));

    }

}

void
idol::Reformulators::KKT::add_variables(idol::Model &t_destination, const std::vector<std::optional<Var>> &t_vars) {

    for (const auto& opt_var : t_vars) {
        if (opt_var.has_value()) {
            t_destination.add(opt_var.value());
        }
    }

}

void
idol::Reformulators::KKT::add_constraints(idol::Model &t_destination, const std::vector<std::optional<Ctr>> &t_ctrs) {

    for (const auto& opt_ctr : t_ctrs) {
        if (opt_ctr.has_value()) {
            t_destination.add(opt_ctr.value());
        }
    }

}

void idol::Reformulators::KKT::add_primal_constraints(idol::Model &t_destination) {

    for (const auto& opt_ctr : m_primal_ctrs) {

        if (!opt_ctr.has_value()) {
            continue;
        }

        const auto& ctr = opt_ctr.value();
        const auto type = m_src_model.get_ctr_type(ctr);
        const auto& src_row = m_src_model.get_ctr_row(ctr);
        auto row = to_destination_space(src_row);

        t_destination.add(ctr, TempCtr(std::move(row), type));

    }

}

void idol::Reformulators::KKT::add_dual_constraints(idol::Model &t_destination) {

    add_constraints(t_destination, m_dual_ctrs);

}

idol::Row idol::Reformulators::KKT::to_destination_space(const idol::Row &t_src_row) const {

    Row result;
    result.rhs() = t_src_row.rhs();

    for (const auto& [var, coeff] : t_src_row.linear()) {

        if (!m_indicator_for_vars(var)) {

            if (m_keep_original_variables) {
                result.linear() += coeff * var;
            } else {
                result.rhs() -= coeff.as_numerical() * !var;
            }

        } else {

            result.linear() += coeff * to_destination_space(var);

        }

    }

    if (!t_src_row.quadratic().empty()) {
        throw Exception("Not implemented.");
    }

    return result;
}
