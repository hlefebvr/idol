//
// Created by henri on 07/02/23.
//
#include "backends/column-generation/Relaxations_DantzigWolfe.h"
#include "modeling/objects/Versions.h"
#include "modeling/expressions/operations/operators.h"

Relaxations::DantzigWolfe::DantzigWolfe(const Model &t_original_model,
                                        Annotation<Ctr, unsigned int> t_complicating_constraint_annotation)
        : m_original_model(t_original_model),
          m_complicating_constraint_annotation(std::move(t_complicating_constraint_annotation)) {

}

void Relaxations::DantzigWolfe::build() {

    const unsigned int n_subproblems = compute_number_of_subproblems();
    m_decomposition.emplace(m_original_model.env(), n_subproblems, m_complicating_constraint_annotation);

    decompose_original_formulation();
    add_convexity_constraints();

}

unsigned int Relaxations::DantzigWolfe::compute_number_of_subproblems() const {

    unsigned int result = 0;
    for (const auto& ctr : m_original_model.ctrs()) {
        const unsigned int subproblem_id = ctr.get(m_complicating_constraint_annotation);
        if (subproblem_id == MasterId) { continue; }
        if (subproblem_id > result) { result = subproblem_id; }
    }

    return result+1;
}

void Relaxations::DantzigWolfe::decompose_original_formulation() {

    for (const auto& ctr : m_original_model.ctrs()) {
        add_variables_to_subproblem(ctr);
    }

    for (const auto& ctr : m_original_model.ctrs()) {
        dispatch_constraint(ctr);
    }

    add_objective_to_master();

}

void Relaxations::DantzigWolfe::add_variables_to_subproblem(const Ctr &t_ctr) {

    const unsigned int subproblem_id = t_ctr.get(m_complicating_constraint_annotation);
    const auto& row = m_original_model.get(Attr::Ctr::Row, t_ctr);

    if (subproblem_id != MasterId) {
        add_variables_to_subproblem(row, subproblem_id);
    }

}

void Relaxations::DantzigWolfe::add_variables_to_subproblem(const Row &t_row, unsigned int t_subproblem_id) {

    for (const auto& [var, constant] : t_row.linear()) {
        add_variable_to_subproblem(var, t_subproblem_id);
    }

    for (const auto& [var1, var2, constant] : t_row.quadratic()) {
        add_variable_to_subproblem(var1, t_subproblem_id);
        add_variable_to_subproblem(var2, t_subproblem_id);
    }

}

void Relaxations::DantzigWolfe::add_variable_to_subproblem(const Var &t_var, unsigned int t_subproblem_id) {

    const auto& annotation = m_decomposition->opposite_axis_annotation();

    if (t_var.get(annotation) != MasterId) {
        if (m_decomposition->subproblem(t_subproblem_id).has(t_var)) {
            return;
        }
        throw Exception("Inconsistent decomposition inferred by annotation " + m_decomposition->axis_annotation().name());
    }

    m_decomposition->subproblem(t_subproblem_id).add(t_var);
    t_var.set(annotation, t_subproblem_id);

}

void Relaxations::DantzigWolfe::dispatch_constraint(const Ctr &t_ctr) {

    const unsigned int subproblem_id = t_ctr.get(m_complicating_constraint_annotation);

    if (subproblem_id == MasterId) {
        add_constraint_and_variables_to_master(t_ctr);
    } else {
        add_constraint_to_subproblem(t_ctr, subproblem_id);
    }

}

void Relaxations::DantzigWolfe::add_constraint_to_subproblem(const Ctr &t_ctr, unsigned int t_subproblem_id) {

    const auto& row = m_original_model.get(Attr::Ctr::Row, t_ctr);
    auto& subproblem = m_decomposition->subproblem(t_subproblem_id);
    subproblem.add(t_ctr);
    subproblem.set(Attr::Ctr::Row, t_ctr, row);

}

void Relaxations::DantzigWolfe::add_constraint_and_variables_to_master(const Ctr &t_ctr) {

    const auto& row = m_original_model.get(Attr::Ctr::Row, t_ctr);
    const auto type = m_original_model.get(Attr::Ctr::Type, t_ctr);

    const unsigned int n_subproblems = m_decomposition->n_subproblems();

    auto [lhs, coefficients] = decompose_master_expression(row.linear());

    m_decomposition->master_problem().add(Ctr(m_original_model.env(), TempCtr(Row(std::move(lhs), row.rhs()), type)));

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        m_decomposition->generation_pattern(i).linear().set(t_ctr, std::move(coefficients[i]));
    }

}

void Relaxations::DantzigWolfe::add_objective_to_master() {

    const auto& objective = m_original_model.get(Attr::Obj::Expr);
    const unsigned int n_subproblems = m_decomposition->n_subproblems();

    auto [master_objective, coefficients] = decompose_master_expression(objective.linear());

    m_decomposition->master_problem().set(Attr::Obj::Expr, std::move(master_objective));

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        m_decomposition->generation_pattern(i).set_obj(std::move(coefficients[i]));
    }

}

void Relaxations::DantzigWolfe::add_variable_to_master(const Var &t_var) {
    auto& master = m_decomposition->master_problem();
    if (!master.has(t_var)) {
        master.add(t_var);
    }
}

std::pair<Expr<Var, Var>, std::vector<Constant>> Relaxations::DantzigWolfe::decompose_master_expression(const LinExpr<Var> &t_expr) {

    const unsigned int n_subproblems = m_decomposition->n_subproblems();

    Expr<Var, Var> master_expr;
    std::vector<Constant> coefficients(n_subproblems);

    const auto& var_annotation = m_decomposition->opposite_axis_annotation();

    for (const auto& [var, constant] : t_expr) {

        const unsigned int subproblem_id = var.get(var_annotation);

        if (subproblem_id == MasterId) {

            add_variable_to_master(var);
            master_expr += constant * var;

        } else {

            if (!constant.is_numerical()) {
                throw Exception("Non-numerical constant found.");
            }

            coefficients[subproblem_id] += constant.numerical() * !var;

        }

    }

    return std::make_pair(std::move(master_expr), std::move(coefficients));

}

void Relaxations::DantzigWolfe::add_convexity_constraints() {

    auto& env = m_original_model.env();
    auto& master = m_decomposition->master_problem();
    const unsigned int n_subproblems = m_decomposition->n_subproblems();

    for (unsigned int i = 0 ; i < n_subproblems ; ++i) {
        Ctr convexity(env, TempCtr(Row(0, 1), Equal), "_convexity_" + std::to_string(i));
        master.add(convexity);
        m_decomposition->set_epigraph(i, convexity);
    }

}
