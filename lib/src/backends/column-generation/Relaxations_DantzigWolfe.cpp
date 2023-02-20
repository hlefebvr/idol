//
// Created by henri on 07/02/23.
//
#include "backends/column-generation/Relaxations_DantzigWolfe.h"
#include "modeling/objects/Versions.h"
#include "modeling/expressions/operations/operators.h"

Relaxations::DantzigWolfe::DantzigWolfe(const AbstractModel &t_original_model,
                                        Annotation<Ctr, unsigned int> t_complicating_constraint_annotation)
        : m_original_model(t_original_model),
          m_complicating_constraint_annotation(std::move(t_complicating_constraint_annotation)) {

}

void Relaxations::DantzigWolfe::build() {

    const unsigned int n_blocks = compute_number_of_blocks();
    m_decomposition.emplace(m_original_model.env(), n_blocks, m_complicating_constraint_annotation);

    decompose_original_formulation();
    add_convexity_constraints();
    make_continuous_relaxation();

}

unsigned int Relaxations::DantzigWolfe::compute_number_of_blocks() const {

    unsigned int result = 0;
    for (const auto& ctr : m_original_model.ctrs()) {
        const unsigned int block_id = ctr.get(m_complicating_constraint_annotation);
        if (block_id == MasterId) { continue; }
        if (block_id > result) { result = block_id; }
    }

    return result+1;
}

void Relaxations::DantzigWolfe::decompose_original_formulation() {

    for (const auto& ctr : m_original_model.ctrs()) {
        add_variables_to_block(ctr);
    }

    m_decomposition->build_opposite_axis();

    for (const auto& ctr : m_original_model.ctrs()) {
        dispatch_constraint(ctr);
    }

    add_objective_to_master();

}

void Relaxations::DantzigWolfe::add_variables_to_block(const Ctr &t_ctr) {

    const unsigned int block_id = t_ctr.get(m_complicating_constraint_annotation);
    const auto& row = m_original_model.get(Attr::Ctr::Row, t_ctr);

    if (block_id != MasterId) {
        add_variables_to_block(row, block_id);
    }

}

void Relaxations::DantzigWolfe::add_variables_to_block(const Row &t_row, unsigned int t_block_id) {

    for (const auto& [var, constant] : t_row.linear()) {
        add_variable_to_block(var, t_block_id);
    }

    for (const auto& [var1, var2, constant] : t_row.quadratic()) {
        add_variable_to_block(var1, t_block_id);
        add_variable_to_block(var2, t_block_id);
    }

}

void Relaxations::DantzigWolfe::add_variable_to_block(const Var &t_var, unsigned int t_block_id) {

    const double lb = m_original_model.get(Attr::Var::Lb, t_var);
    const double ub = m_original_model.get(Attr::Var::Ub, t_var);
    const int type = m_original_model.get(Attr::Var::Type, t_var);

    auto& block_model = m_decomposition->block(t_block_id).model();

    if (!block_model.has(t_var)) {
        block_model.add(t_var, TempVar(lb, ub, type, Column()));
    }
}

void Relaxations::DantzigWolfe::dispatch_constraint(const Ctr &t_ctr) {

    const unsigned int block_id = t_ctr.get(m_complicating_constraint_annotation);

    if (block_id == MasterId) {
        add_constraint_and_variables_to_master(t_ctr);
    } else {
        add_constraint_to_block(t_ctr, block_id);
    }

}

void Relaxations::DantzigWolfe::add_constraint_to_block(const Ctr &t_ctr, unsigned int t_block_id) {

    const auto& row = m_original_model.get(Attr::Ctr::Row, t_ctr);
    const auto type = m_original_model.get(Attr::Ctr::Type, t_ctr);
    auto& block = m_decomposition->block(t_block_id);
    block.model().add(t_ctr, TempCtr(Row(row), type));

}

void Relaxations::DantzigWolfe::add_constraint_and_variables_to_master(const Ctr &t_ctr) {

    const auto& row = m_original_model.get(Attr::Ctr::Row, t_ctr);
    const auto type = m_original_model.get(Attr::Ctr::Type, t_ctr);

    const unsigned int n_blocks = m_decomposition->n_blocks();

    auto [lhs, coefficients] = decompose_master_expression(row.linear());

    for (const auto& [var1, var2, constant] : row.quadratic()) {

        add_variable_to_master(var1);
        add_variable_to_master(var2);

        lhs += constant * var1 * var2;
    }

    m_decomposition->master().add(t_ctr, TempCtr(Row(std::move(lhs), row.rhs()), type));

    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        m_decomposition->block(i).generation_pattern().linear().set(t_ctr, std::move(coefficients[i]));
    }

}

void Relaxations::DantzigWolfe::add_objective_to_master() {

    const auto& objective = m_original_model.get(Attr::Obj::Expr);
    const unsigned int n_blocks = m_decomposition->n_blocks();

    auto [master_objective, coefficients] = decompose_master_expression(objective.linear());

    for (const auto& [var1, var2, constant] : objective.quadratic()) {
        add_variable_to_master(var1);
        add_variable_to_master(var2);

        master_objective += constant * var1 * var2;
    }

    m_decomposition->master().set(Attr::Obj::Expr, std::move(master_objective));

    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        m_decomposition->block(i).generation_pattern().set_obj(std::move(coefficients[i]));
    }

}

void Relaxations::DantzigWolfe::add_variable_to_master(const Var &t_var) {
    if (!m_decomposition->has(t_var)) {
        const double lb = m_original_model.get(Attr::Var::Lb, t_var);
        const double ub = m_original_model.get(Attr::Var::Ub, t_var);
        const int type = m_original_model.get(Attr::Var::Type, t_var);
        m_decomposition->master().add(t_var, TempVar(lb, ub, type, Column()));
    }
}

std::pair<Expr<Var, Var>, std::vector<Constant>> Relaxations::DantzigWolfe::decompose_master_expression(const LinExpr<Var> &t_expr) {

    const unsigned int n_blocks = m_decomposition->n_blocks();

    Expr<Var, Var> master_expr;
    std::vector<Constant> coefficients(n_blocks);

    const auto& var_annotation = m_decomposition->opposite_axis();

    for (const auto& [var, constant] : t_expr) {

        const unsigned int block_id = var.get(var_annotation);

        if (block_id == MasterId) {

            add_variable_to_master(var);
            master_expr += constant * var;

        } else {

            if (!constant.is_numerical()) {
                throw Exception("Non-numerical constant found.");
            }

            coefficients[block_id] += constant.numerical() * !var;

        }

    }

    return std::make_pair(std::move(master_expr), std::move(coefficients));

}

void Relaxations::DantzigWolfe::add_convexity_constraints() {

    auto& env = m_original_model.env();
    const unsigned int n_blocks = m_decomposition->n_blocks();

    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        Ctr convexity(env, TempCtr(Row(0, 1), Equal), "_convexity_" + std::to_string(i));
        m_decomposition->master().add(convexity);
        m_decomposition->block(i).generation_pattern().linear().set(convexity, 1);
        m_decomposition->block(i).set_aggregator(convexity);
    }

}

void Relaxations::DantzigWolfe::make_continuous_relaxation() {

    make_continuous_relaxation_of_master(m_decomposition->master());
    for (auto& block : m_decomposition->blocks()) {
        add_integer_variables_to_branching_candidates(block.model());
    }

}

void Relaxations::DantzigWolfe::make_continuous_relaxation_of_master(Model &t_model) {

    for (const auto& var : t_model.vars()) {
        if (t_model.get(Attr::Var::Type, var) != Continuous) {
            t_model.set(Attr::Var::Type, var, Continuous);
            m_branching_candidates.emplace_back(var);
        }
    }

}

void Relaxations::DantzigWolfe::add_integer_variables_to_branching_candidates(Model &t_model) {

    for (const auto& var : t_model.vars()) {
        if (t_model.get(Attr::Var::Type, var) != Continuous) {
            m_branching_candidates.emplace_back(var);
        }
    }

}
