//
// Created by henri on 04/01/23.
//

#include "reformulations/Reformulations_Benders.h"
#include "modeling/expressions/operations/operators.h"

Reformulations::Benders::Benders(Model &t_original_formulation, UserAttr t_subproblem_flag)
        : m_original_formulation(t_original_formulation),
          m_subproblem_flag(std::move(t_subproblem_flag)),
          m_reformulated_variable(t_original_formulation.add_user_attr<Var>(Var(), "ReformulatedVariable")),
          m_reformulated_constraint(t_original_formulation.add_user_attr<Ctr>(Ctr(), "ReformulatedConstraint")) {

    const unsigned int n_subproblems = compute_number_of_subproblems();

    detect_subproblem_constraints();

    m_subproblems.resize(n_subproblems);
    m_thetas.resize(n_subproblems);
    m_benders_cuts.resize(n_subproblems);
    m_original_variable.reserve(n_subproblems);
    m_original_constraint.reserve(n_subproblems);

    create_original_space_user_attributes();
    create_variables();
    create_constraints();

}

unsigned int Reformulations::Benders::compute_number_of_subproblems() const {
    unsigned int result = 1;

    for (const auto& var : m_original_formulation.vars()) {
        if (const unsigned int id = m_original_formulation.get<unsigned int>(m_subproblem_flag, var) ; id > result) {
            result = id;
        }
    }

    return result;
}

void Reformulations::Benders::detect_subproblem_constraints() {

    for (const auto& var : m_original_formulation.vars()) {

        const unsigned int model_id = m_original_formulation.get<unsigned int>(m_subproblem_flag, var);

        if (model_id == 0) { continue; }

        detect_subproblem_constraints(var, model_id);

    }

}

void Reformulations::Benders::detect_subproblem_constraints(const Var &t_var, unsigned int t_model_id) {

    const auto& col = m_original_formulation.get(Attr::Var::Column, t_var);

    for (const auto& [ctr, constant] : col.linear()) {

        throw_if_inconsistently_dispatched(t_var, ctr, t_model_id);

        m_original_formulation.set<unsigned int>(m_subproblem_flag, ctr, t_model_id);

    }

    if (!col.quadratic().empty()) {
        throw Exception("Reformulating quadratic models is not implemented");
    }

}

void Reformulations::Benders::throw_if_inconsistently_dispatched(const Var &t_var, const Ctr &t_ctr, unsigned int t_model_id) {

    if (unsigned int id = m_original_formulation.get<unsigned int>(m_subproblem_flag, t_ctr) ; id != 0 && id != t_model_id) {
        throw Exception("Inconsistent decomposition found. (Constraint " + t_ctr.name()
                        + " with subproblem id " + std::to_string(id)
                        + " was found in variable " + t_var.name() + " with subproblem id " + std::to_string(t_model_id) );
    }

}

void Reformulations::Benders::create_original_space_user_attributes() {

    for (auto& subproblem : subproblems()) {
        m_original_variable.emplace_back(subproblem.add_user_attr<Var>(Var(), "original_space_var"));
        m_original_constraint.emplace_back(subproblem.add_user_attr<Ctr>(Ctr(), "original_space_Ctr"));
    }

}

void Reformulations::Benders::create_constraints() {

    const unsigned int n_subproblems = m_subproblems.size();
    std::vector<Expr<Var, Var>> benders_cut_expressions(n_subproblems);

    for (const auto& ctr : m_original_formulation.ctrs()) {

        const unsigned int model_id = m_original_formulation.get<unsigned int>(m_subproblem_flag, ctr);
        const auto type = m_original_formulation.get(Attr::Ctr::Type, ctr);
        const auto& original_row = m_original_formulation.get(Attr::Ctr::Row, ctr);

        auto created_ctr = model(model_id).add_ctr(TempCtr(Row(), type), ctr.name() + "__r" + std::to_string(model_id));
        m_original_formulation.set<Ctr>(m_reformulated_constraint, ctr, created_ctr);

        if (model_id == 0) {
            add_row_master(created_ctr, original_row);
        } else {
            add_row_subproblem(created_ctr, original_row, benders_cut_expressions.at(model_id - 1), model_id);
        }

    }

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        m_thetas[i] = m_master_problem.add_var(-Inf, Inf, Continuous, 1., "_bnd_" + std::to_string(i+1));
        m_benders_cuts[i] = m_master_problem.add_ctr(m_thetas.at(i) >= benders_cut_expressions.at(i), "_bnd_cut_" + std::to_string(i+1));
    }

}

Model &Reformulations::Benders::model(unsigned int t_model_id) {
    return t_model_id == 0 ? m_master_problem : m_subproblems[t_model_id - 1];
}

void Reformulations::Benders::create_variables() {

    for (const auto& var : m_original_formulation.vars()) {

        const unsigned int model_id = m_original_formulation.get<unsigned int>(m_subproblem_flag, var);
        const double lb = m_original_formulation.get(Attr::Var::Lb, var);
        const double ub = m_original_formulation.get(Attr::Var::Ub, var);
        const int type = m_original_formulation.get(Attr::Var::Type, var);
        const auto& obj_coeff = m_original_formulation.get(Attr::Var::Obj, var);

        auto created_var = model(model_id).add_var(lb, ub, type, obj_coeff, var.name() + "__r" + std::to_string(model_id));

        m_original_formulation.set<Var>(m_reformulated_variable, var, created_var);

        if (model_id > 0) {
            model(model_id).set<Var>(m_original_variable[model_id-1], created_var, var);
        }

    }


}

void Reformulations::Benders::add_row_master(const Ctr& t_created_ctr, const Row& t_original_row) {
    Row result;

    for (const auto& [var, constant] : t_original_row.linear()) {
        result.linear() += constant * m_original_formulation.get<Var>(m_reformulated_variable, var);
    }

    result.rhs() = t_original_row.rhs();

    m_master_problem.set(Attr::Ctr::Row, t_created_ctr, std::move(result));
}

void Reformulations::Benders::add_row_subproblem(const Ctr& t_created_ctr, const Row& t_original_row, Expr<Var, Var>& t_benders_cut, unsigned int t_model_id) {

    Row result;

    if (!t_original_row.rhs().is_numerical()) {
        throw Exception("Cannot handle parametrized constant in Benders reformulation.");
    }

    t_benders_cut += t_original_row.rhs().numerical() * !t_created_ctr;

    for (const auto& [var, constant] : t_original_row.linear()) {
        if (m_original_formulation.get<unsigned int>(m_subproblem_flag, var) == 0) {
            t_benders_cut += -constant.numerical() * !t_created_ctr * m_original_formulation.get<Var>(m_reformulated_variable, var);
        } else {
            result.linear() += constant * m_original_formulation.get<Var>(m_reformulated_variable, var);
        }
    }

    model(t_model_id).set(Attr::Ctr::Row, t_created_ctr, std::move(result));

}
