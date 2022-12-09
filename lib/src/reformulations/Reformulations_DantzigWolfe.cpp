//
// Created by henri on 09/12/22.
//
#include "reformulations/Reformulations_DantzigWolfe.h"

Reformulations::DantzigWolfe::DantzigWolfe(Model &t_original_formulation, UserAttr t_is_complicating)
        : m_original_formulation(t_original_formulation),
          m_is_complicating(std::move(t_is_complicating)),
          m_reformulated_variable(t_original_formulation.add_user_attr<Var>(Var(), "ReformulatedVariable")),
          m_reformulated_constraint(t_original_formulation.add_user_attr<Ctr>(Ctr(), "ReformulatedConstraint")) {

    const unsigned int n_subproblems = compute_number_of_subproblems();

    detect_subproblem_variables();

    m_subproblems.resize(n_subproblems);
    m_alphas.resize(n_subproblems);
    m_convexity_constraints.resize(n_subproblems);

    create_alphas();
    create_variables();
    create_constraints();

}

Model &Reformulations::DantzigWolfe::model(unsigned int t_model_id) {
    return t_model_id == 0 ? m_restricted_master_problem : m_subproblems[t_model_id - 1];
}

unsigned int Reformulations::DantzigWolfe::compute_number_of_subproblems() const {
    unsigned int result = 1;

    for (const auto& ctr : m_original_formulation.ctrs()) {
        if (const unsigned int id = m_original_formulation.get<unsigned int>(m_is_complicating, ctr) ; id > result) {
            result = id;
        }
    }

    return result;
}

void Reformulations::DantzigWolfe::detect_subproblem_variables() {

    for (const auto& ctr : m_original_formulation.ctrs()) {

        const unsigned int model_id = m_original_formulation.get<unsigned int>(m_is_complicating, ctr);

        if (model_id == 0) { continue; }

        detect_subproblem_variables(ctr, model_id);

    }

}

void Reformulations::DantzigWolfe::throw_if_inconsistently_dispatched(const Ctr &t_ctr, const Var &t_var,
                                                                      unsigned int t_model_id) {

    if (unsigned int id = m_original_formulation.get<unsigned int>(m_is_complicating, t_var) ; id != 0 && id != t_model_id) {
        throw Exception("Inconsistent decomposition found. (Variable " + t_var.name()
                        + " with subproblem id " + std::to_string(id)
                        + " was found in constraint " + t_ctr.name() + " with subproblem id " + std::to_string(t_model_id) );
    }

}

void Reformulations::DantzigWolfe::detect_subproblem_variables(const Ctr &t_ctr, unsigned int t_model_id) {

    const auto& row = m_original_formulation.get(Attr::Ctr::Row, t_ctr);

    for (const auto& [var, constant] : row.linear()) {

        throw_if_inconsistently_dispatched(t_ctr, var, t_model_id);

        m_original_formulation.set<unsigned int>(m_is_complicating, var, t_model_id);
    }

    for (const auto& [var1, var2, constant] : row.quadratic()) {

        throw_if_inconsistently_dispatched(t_ctr, var1, t_model_id);
        throw_if_inconsistently_dispatched(t_ctr, var2, t_model_id);

        m_original_formulation.set<unsigned int>(m_is_complicating, var1, t_model_id);
        m_original_formulation.set<unsigned int>(m_is_complicating, var2, t_model_id);

    }

}

void Reformulations::DantzigWolfe::create_alphas() {

    for (unsigned int i = 0, n = m_subproblems.size() ; i < n ; ++i) {
        m_alphas[i] = m_restricted_master_problem.add_var(0., Inf, Continuous, 0., "_dw_" + std::to_string(i+1));
        m_restricted_master_problem.add_ctr(m_alphas[i] == 1, "_dw_conv_" + std::to_string(i));
    }

}

void Reformulations::DantzigWolfe::create_variables() {

    Expr objective;

    for (const auto& var : m_original_formulation.vars()) {

        const unsigned int model_id = m_original_formulation.get<unsigned int>(m_is_complicating, var);
        const double lb = m_original_formulation.get(Attr::Var::Lb, var);
        const double ub = m_original_formulation.get(Attr::Var::Ub, var);
        const int type = m_original_formulation.get(Attr::Var::Type, var);
        const auto& obj_coeff = m_original_formulation.get(Attr::Var::Obj, var);

        auto created_var = model(model_id).add_var(lb, ub, type, 0, var.name());

        m_original_formulation.set<Var>(m_reformulated_variable, var, created_var);

        if (model_id == 0) {
            objective += obj_coeff * created_var;
        } else {
            if (!obj_coeff.is_numerical()) {
                throw Exception("Unable to handle parametrized constant for subproblem variables.");
            }
            objective += obj_coeff.numerical() * !created_var * alpha(model_id);
        }

    }

    m_restricted_master_problem.set(Attr::Obj::Expr, objective);

}

void Reformulations::DantzigWolfe::create_constraints() {

    for (const auto& ctr : m_original_formulation.ctrs()) {
        const unsigned int model_id = m_original_formulation.get<unsigned int>(m_is_complicating, ctr);
        if (model_id == 0) {
            create_rmp_constraint(ctr);
        } else {
            create_subproblem_constraint(ctr, model_id);
        }
    }

}

void Reformulations::DantzigWolfe::create_rmp_constraint(const Ctr &t_ctr) {

    const auto& row = m_original_formulation.get(Attr::Ctr::Row, t_ctr);
    const int type = m_original_formulation.get(Attr::Ctr::Type, t_ctr);

    Expr lhs;

    for (const auto& [var, constant] : row.linear()) {
        if (unsigned int model_id = m_original_formulation.get<unsigned int>(m_is_complicating, var) ; model_id == 0) {
            lhs += constant * m_original_formulation.get<Var>(m_reformulated_variable, var);
        } else {
            if (!constant.is_numerical()) {
                throw Exception("Unable to handle parametrized constant for subproblem variables.");
            }
            lhs += constant.numerical() * !m_original_formulation.get<Var>(m_reformulated_variable, var) * alpha(model_id);
        }
    }

    m_restricted_master_problem.add_ctr(TempCtr(Row(lhs, row.rhs()), type), t_ctr.name());

}

void Reformulations::DantzigWolfe::create_subproblem_constraint(const Ctr &t_ctr, unsigned int t_model_id) {

    const auto& row = m_original_formulation.get(Attr::Ctr::Row, t_ctr);
    const int type = m_original_formulation.get(Attr::Ctr::Type, t_ctr);

    Expr lhs;

    for (const auto& [var, constant] : row.linear()) {
        lhs += constant * m_original_formulation.get<Var>(m_reformulated_variable, var);
    }

    for (const auto& [var1, var2, constant] : row.quadratic()) {
        lhs += constant * m_original_formulation.get<Var>(m_reformulated_variable, var1)
               * m_original_formulation.get<Var>(m_reformulated_variable, var2);
    }

    auto created_constraint = model(t_model_id).add_ctr(TempCtr(Row(std::move(lhs), row.rhs()), type), t_ctr.name());
    m_original_formulation.set<Ctr>(m_reformulated_constraint, t_ctr, created_constraint);

}
