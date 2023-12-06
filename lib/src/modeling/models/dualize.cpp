//
// Created by henri on 06.12.23.
//
#include <idol/modeling/models/dualize.h>
#include "idol/modeling/expressions/operations/operators.h"

namespace idol {
    class Dualizer;
}

class idol::Dualizer {
    Env& m_env;
    const Model& m_primal;
    Model& m_dual;

    std::vector<Var> m_dual_variables_for_constraints;
    std::vector<Var> m_dual_variables_for_lower_bounds;
    std::vector<Var> m_dual_variables_for_upper_bounds;

    void create_dual_variables_for_constraints();
    void create_dual_variables_for_lower_bounds();
    void create_dual_variables_for_upper_bounds();

    [[nodiscard]] Var dual(const Ctr& t_ctr) const;
    [[nodiscard]] Var dual_lb(const Var& t_var) const;
    [[nodiscard]] Var dual_ub(const Var& t_var) const;

    [[nodiscard]] double dual_var_lb_for_ctr_type(const CtrType& t_type) const;
    [[nodiscard]] double dual_var_ub_for_ctr_type(const CtrType& t_type) const;
public:
    Dualizer(const Model& t_primal, Model& t_dual);

    void dualize();
};

idol::Dualizer::Dualizer(const idol::Model &t_primal, idol::Model &t_dual)
    : m_env(t_primal.env()),
      m_primal(t_primal),
      m_dual(t_dual) {

    create_dual_variables_for_constraints();
    create_dual_variables_for_lower_bounds();
    create_dual_variables_for_upper_bounds();

}

void idol::Dualizer::dualize() {

    m_dual.set_obj_sense(m_primal.get_obj_sense() == Minimize ? Maximize : Minimize);

    m_dual.add_vector<Var, 1>(m_dual_variables_for_constraints);
    m_dual.add_vector<Var, 1>(m_dual_variables_for_lower_bounds);
    m_dual.add_vector<Var, 1>(m_dual_variables_for_upper_bounds);

    m_dual.set_obj_expr(
        idol_Sum(ctr, m_primal.ctrs(), m_primal.get_ctr_row(ctr).rhs() * dual(ctr))
        + idol_Sum(var, m_primal.vars(), m_primal.get_var_lb(var) * dual_lb(var))
        + idol_Sum(var, m_primal.vars(), m_primal.get_var_ub(var) * dual_ub(var))
    );

    for (const auto& var : m_primal.vars()) {
        auto lhs = idol_Sum(ctr, m_primal.ctrs(), m_primal.get_ctr_row(ctr).linear().get(var) * dual(ctr))
                            + dual_lb(var)
                            + dual_ub(var);
        m_dual.add_ctr(std::move(lhs) == m_primal.get_obj_expr().linear().get(var), "dual_" + var.name());
    }

    for (const auto& var : m_primal.vars()) {

        const double lb = m_primal.get_var_lb(var);
        const double ub = m_primal.get_var_ub(var);

        if (is_neg_inf(lb)) {
            m_dual.remove(dual_lb(var));
        }

        if (is_pos_inf(ub)) {
            m_dual.remove(dual_ub(var));
        }

    }

}

void idol::Dualizer::create_dual_variables_for_constraints() {
    const unsigned int n_constraints = m_primal.ctrs().size();
    m_dual_variables_for_constraints.reserve(n_constraints);
    for (unsigned int i = 0 ; i < n_constraints ; ++i) {

        const auto& ctr = m_primal.get_ctr_by_index(i);

        if (!m_primal.get_ctr_row(ctr).quadratic().empty()) {
            throw Exception("Only LPs can be dualized.");
        }

        const auto type = m_primal.get_ctr_type(ctr);
        double lb = dual_var_lb_for_ctr_type(type);
        double ub = dual_var_ub_for_ctr_type(type);
        m_dual_variables_for_constraints.emplace_back(m_env, lb, ub, Continuous, "dual_" + ctr.name());

    }
}

double idol::Dualizer::dual_var_lb_for_ctr_type(const idol::CtrType &t_type) const {

    if (m_primal.get_obj_sense() == Maximize) {
        return t_type == LessOrEqual ? 0 : -Inf;
    }

    return t_type == GreaterOrEqual ? 0 : -Inf;
}

double idol::Dualizer::dual_var_ub_for_ctr_type(const idol::CtrType &t_type) const {

    if (m_primal.get_obj_sense() == Maximize) {
        return t_type == GreaterOrEqual ? 0 : Inf;
    }

    return t_type == LessOrEqual ? 0 : Inf;
}

void idol::Dualizer::create_dual_variables_for_lower_bounds() {
    const unsigned int n_variables = m_primal.vars().size();
    m_dual_variables_for_lower_bounds.reserve(n_variables);
    for (unsigned int i = 0 ; i < n_variables ; ++i) {
        const auto &var = m_primal.get_var_by_index(i);
        double lb = dual_var_lb_for_ctr_type(GreaterOrEqual);
        double ub = dual_var_ub_for_ctr_type(GreaterOrEqual);
        m_dual_variables_for_lower_bounds.emplace_back(m_env, lb, ub, Continuous, "dual_lb_" + var.name());
    }
}

void idol::Dualizer::create_dual_variables_for_upper_bounds() {
    const unsigned int n_variables = m_primal.vars().size();
    m_dual_variables_for_upper_bounds.reserve(n_variables);
    for (unsigned int i = 0 ; i < n_variables ; ++i) {
        const auto &var = m_primal.get_var_by_index(i);
        double lb = dual_var_lb_for_ctr_type(LessOrEqual);
        double ub = dual_var_ub_for_ctr_type(LessOrEqual);
        m_dual_variables_for_upper_bounds.emplace_back(m_env, lb, ub, Continuous, "dual_ub_" + var.name());
    }
}

idol::Var idol::Dualizer::dual(const idol::Ctr &t_ctr) const {
    return m_dual_variables_for_constraints.at(m_primal.get_ctr_index(t_ctr));
}

idol::Var idol::Dualizer::dual_lb(const idol::Var &t_var) const {
    return m_dual_variables_for_lower_bounds.at(m_primal.get_var_index(t_var));
}

idol::Var idol::Dualizer::dual_ub(const idol::Var &t_var) const {
    return m_dual_variables_for_upper_bounds.at(m_primal.get_var_index(t_var));
}

idol::Model idol::dualize(const idol::Model& t_primal) {

    Model result(t_primal.env());

    Dualizer dualizer(t_primal, result);

    dualizer.dualize();

    return std::move(result);
}