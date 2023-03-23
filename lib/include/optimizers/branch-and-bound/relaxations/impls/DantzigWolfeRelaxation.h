//
// Created by henri on 22/03/23.
//

#ifndef IDOL_DANTZIGWOLFERELAXATION_H
#define IDOL_DANTZIGWOLFERELAXATION_H

#include "optimizers/branch-and-bound/relaxations/factories/RelaxationBuilderFactory.h"
#include "modeling/models/BlockModel.h"
#include "modeling/objects/Versions.h"
#include "modeling/expressions/operations/operators.h"

class DantzigWolfeRelaxation : public RelaxationBuilderFactory {
    Annotation<Ctr, unsigned int> m_decomposition;

    [[nodiscard]] unsigned int compute_number_of_blocs(const AbstractModel& t_model) const;
    void decompose_original_formulation(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model) const;
    void add_variables_to_block(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Ctr& t_ctr) const;
    void add_variables_to_block(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Row& t_row, unsigned int t_block_id) const;
    void add_variable_to_block(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Var& t_var, unsigned int t_block_id) const;
    void dispatch_constraint(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Ctr& t_ctr) const;
    void add_constraint_to_block(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Ctr& t_ctr, unsigned int t_block_id) const;
    void add_constraint_and_variables_to_master(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Ctr& t_ctr) const;
    void add_variable_to_master(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const Var& t_var) const;
    void add_objective_to_master(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model) const;
    std::pair<Expr<Var, Var>, std::vector<Constant>> decompose_master_expression(const AbstractModel& t_original_model, BlockModel<Ctr>& t_model, const LinExpr<Var>& t_expr) const;
    void add_convexity_constraints(BlockModel<Ctr>& t_model) const;
    void make_continuous_relaxation(BlockModel<Ctr>& t_model) const;
public:
    explicit DantzigWolfeRelaxation(const Annotation<Ctr, unsigned int>& t_decomposition)
        : m_decomposition(t_decomposition) {}

    BlockModel<Ctr> *operator()(const AbstractModel &t_model) const override {

        const unsigned int n_blocks = compute_number_of_blocs(t_model);

        auto* result = new BlockModel<Ctr>(t_model.env(), n_blocks, m_decomposition);

        decompose_original_formulation(t_model, *result);
        add_convexity_constraints(*result);
        make_continuous_relaxation(*result);

        return result;
    }

    [[nodiscard]] DantzigWolfeRelaxation *clone() const override {
        return new DantzigWolfeRelaxation(*this);
    }
};

unsigned int DantzigWolfeRelaxation::compute_number_of_blocs(const AbstractModel &t_model) const {

    unsigned int result = 0;
    for (const auto& ctr : t_model.ctrs()) {
        const unsigned int block_id = ctr.get(m_decomposition);
        if (block_id == MasterId) { continue; }
        if (block_id > result) { result = block_id; }
    }

    return result+1;
}

void DantzigWolfeRelaxation::decompose_original_formulation(const AbstractModel &t_original_model,
                                                            BlockModel<Ctr> &t_model) const {

    for (const auto& ctr : t_original_model.ctrs()) {
        add_variables_to_block(t_original_model, t_model, ctr);
    }

    t_model.build_opposite_axis();

    for (const auto& ctr : t_original_model.ctrs()) {
        dispatch_constraint(t_original_model, t_model, ctr);
    }

    add_objective_to_master(t_original_model, t_model);

}

void DantzigWolfeRelaxation::add_variables_to_block(const AbstractModel &t_original_model,
                                                    BlockModel<Ctr> &t_model,
                                                    const Ctr &t_ctr) const {

    const unsigned int block_id = t_ctr.get(m_decomposition);
    const auto& row = t_original_model.get(Attr::Ctr::Row, t_ctr);

    if (block_id != MasterId) {
        add_variables_to_block(t_original_model, t_model, row, block_id);
    }


}

void DantzigWolfeRelaxation::add_variables_to_block(const AbstractModel &t_original_model,
                                                    BlockModel<Ctr> &t_model,
                                                    const Row &t_row, unsigned int t_block_id) const {


    for (const auto& [var, constant] : t_row.linear()) {
        add_variable_to_block(t_original_model, t_model, var, t_block_id);
    }

    for (const auto& [var1, var2, constant] : t_row.quadratic()) {
        add_variable_to_block(t_original_model, t_model, var1, t_block_id);
        add_variable_to_block(t_original_model, t_model, var2, t_block_id);
    }


}

void DantzigWolfeRelaxation::add_variable_to_block(const AbstractModel &t_original_model,
                                                   BlockModel<Ctr> &t_model,
                                                   const Var &t_var,
                                                   unsigned int t_block_id) const {

    const double lb = t_original_model.get(Attr::Var::Lb, t_var);
    const double ub = t_original_model.get(Attr::Var::Ub, t_var);
    const int type = t_original_model.get(Attr::Var::Type, t_var);

    auto& block_model = t_model.block(t_block_id).model();

    if (!block_model.has(t_var)) {
        block_model.add(t_var, TempVar(lb, ub, type, Column()));
    }

}

void DantzigWolfeRelaxation::dispatch_constraint(const AbstractModel &t_original_model,
                                                 BlockModel<Ctr> &t_model,
                                                 const Ctr &t_ctr) const {

    const unsigned int block_id = t_ctr.get(m_decomposition);

    if (block_id == MasterId) {
        add_constraint_and_variables_to_master(t_original_model, t_model, t_ctr);
    } else {
        add_constraint_to_block(t_original_model, t_model, t_ctr, block_id);
    }

}

void DantzigWolfeRelaxation::add_constraint_to_block(const AbstractModel &t_original_model,
                                                     BlockModel<Ctr> &t_model,
                                                     const Ctr &t_ctr, unsigned int t_block_id) const {

    const auto& row = t_original_model.get(Attr::Ctr::Row, t_ctr);
    const auto type = t_original_model.get(Attr::Ctr::Type, t_ctr);
    auto& block = t_model.block(t_block_id);
    block.model().add(t_ctr, TempCtr(Row(row), type));

}

void DantzigWolfeRelaxation::add_constraint_and_variables_to_master(const AbstractModel &t_original_model,
                                                                    BlockModel<Ctr> &t_model,
                                                                    const Ctr &t_ctr) const {

    const auto& row = t_original_model.get(Attr::Ctr::Row, t_ctr);
    const auto type = t_original_model.get(Attr::Ctr::Type, t_ctr);

    const unsigned int n_blocks = t_model.n_blocks();

    auto [lhs, coefficients] = decompose_master_expression(t_original_model, t_model, row.linear());

    for (const auto& [var1, var2, constant] : row.quadratic()) {

        add_variable_to_master(t_original_model, t_model, var1);
        add_variable_to_master(t_original_model, t_model, var2);

        lhs += constant * var1 * var2;
    }

    t_model.master().add(t_ctr, TempCtr(Row(std::move(lhs), row.rhs()), type));

    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        t_model.block(i).generation_pattern().linear().set(t_ctr, std::move(coefficients[i]));
    }

}

void DantzigWolfeRelaxation::add_variable_to_master(const AbstractModel &t_original_model,
                                                    BlockModel<Ctr> &t_model,
                                                    const Var &t_var) const {

    if (!t_model.has(t_var)) {
        const double lb = t_original_model.get(Attr::Var::Lb, t_var);
        const double ub = t_original_model.get(Attr::Var::Ub, t_var);
        const int type = t_original_model.get(Attr::Var::Type, t_var);
        t_model.master().add(t_var, TempVar(lb, ub, type, Column()));
    }

}

void DantzigWolfeRelaxation::add_objective_to_master(const AbstractModel &t_original_model,
                                                BlockModel<Ctr> &t_model) const {

    const auto& objective = t_original_model.get(Attr::Obj::Expr);
    const unsigned int n_blocks = t_model.n_blocks();

    auto [master_objective, coefficients] = decompose_master_expression(t_original_model, t_model, objective.linear());

    for (const auto& [var1, var2, constant] : objective.quadratic()) {
        add_variable_to_master(t_original_model, t_model, var1);
        add_variable_to_master(t_original_model, t_model, var2);

        master_objective += constant * var1 * var2;
    }

    t_model.master().set(Attr::Obj::Expr, std::move(master_objective));

    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        t_model.block(i).generation_pattern().set_obj(std::move(coefficients[i]));
    }

}

std::pair<Expr<Var, Var>, std::vector<Constant>>
DantzigWolfeRelaxation::decompose_master_expression(const AbstractModel &t_original_model, BlockModel<Ctr> &t_model,
                                                    const LinExpr<Var> &t_expr) const {

    const unsigned int n_blocks = t_model.n_blocks();

    Expr<Var, Var> master_expr;
    std::vector<Constant> coefficients(n_blocks);

    const auto& var_annotation = t_model.opposite_axis();

    for (const auto& [var, constant] : t_expr) {

        const unsigned int block_id = var.get(var_annotation);

        if (block_id == MasterId) {

            add_variable_to_master(t_original_model, t_model, var);
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

void DantzigWolfeRelaxation::add_convexity_constraints(BlockModel<Ctr> &t_model) const {

    auto& env = t_model.env();
    const unsigned int n_blocks = t_model.n_blocks();

    for (unsigned int i = 0 ; i < n_blocks ; ++i) {
        Ctr convexity(env, TempCtr(Row(0, 1), Equal), "_convexity_" + std::to_string(i));
        t_model.master().add(convexity);
        t_model.block(i).generation_pattern().linear().set(convexity, 1);
        t_model.block(i).set_aggregator(convexity);
    }

}

void DantzigWolfeRelaxation::make_continuous_relaxation(BlockModel<Ctr> &t_model) const {

    for (const auto& var : t_model.vars()) {
        if (t_model.get(Attr::Var::Type, var) != Continuous) {
            t_model.set(Attr::Var::Type, var, Continuous);
        }
    }

}

#endif //IDOL_DANTZIGWOLFERELAXATION_H
