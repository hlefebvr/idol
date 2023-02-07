//
// Created by henri on 07/02/23.
//

#ifndef IDOL_RELAXATIONS_DANTZIGWOLFE_H
#define IDOL_RELAXATIONS_DANTZIGWOLFE_H

#include "backends/Relaxation.h"
#include "modeling/models/BlockModel.h"

namespace Relaxations {
    class DantzigWolfe;
}

class Relaxations::DantzigWolfe : public Relaxation {
    const AbstractModel& m_original_model;
    Annotation<Ctr, unsigned int> m_complicating_constraint_annotation;
    std::optional<BlockModel<Ctr>> m_decomposition;
protected:
    [[nodiscard]] unsigned int compute_number_of_blocks() const;
    void decompose_original_formulation();
    void add_variables_to_block(const Ctr& t_ctr);
    void add_variables_to_block(const Row& t_row, unsigned int t_block_id);
    void add_variable_to_block(const Var& t_var, unsigned int t_block_id);
    void dispatch_constraint(const Ctr& t_ctr);
    void add_constraint_to_block(const Ctr& t_ctr, unsigned int t_block_id);
    void add_constraint_and_variables_to_master(const Ctr& t_ctr);
    void add_variable_to_master(const Var& t_var);
    void add_objective_to_master();
    std::pair<Expr<Var, Var>, std::vector<Constant>> decompose_master_expression(const LinExpr<Var>& t_expr);
    void add_convexity_constraints();
public:
    DantzigWolfe(const AbstractModel& t_original_model, Annotation<Ctr, unsigned int> t_complicating_constraint_annotation);

    void build() override;

    BlockModel<Ctr> &model() override { return m_decomposition.value(); }

    [[nodiscard]] const BlockModel<Ctr> &model() const override { return m_decomposition.value(); }
};

#endif //IDOL_RELAXATIONS_DANTZIGWOLFE_H
