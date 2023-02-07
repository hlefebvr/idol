//
// Created by henri on 07/02/23.
//

#ifndef IDOL_RELAXATIONS_DANTZIGWOLFE_H
#define IDOL_RELAXATIONS_DANTZIGWOLFE_H

#include "backends/Relaxation.h"
#include "modeling/models/Decomposition.h"

namespace Relaxations {
    class DantzigWolfe;
}

class Relaxations::DantzigWolfe : public Relaxation {
    const Model& m_original_model;
    Annotation<Ctr, unsigned int> m_complicating_constraint_annotation;
    std::optional<Decomposition<Ctr>> m_decomposition;
protected:
    [[nodiscard]] unsigned int compute_number_of_subproblems() const;
    void decompose_original_formulation();
    void add_variables_to_subproblem(const Ctr& t_ctr);
    void add_variables_to_subproblem(const Row& t_row, unsigned int t_subproblem_id);
    void add_variable_to_subproblem(const Var& t_var, unsigned int t_subproblem_id);
    void dispatch_constraint(const Ctr& t_ctr);
    void add_constraint_to_subproblem(const Ctr& t_ctr, unsigned int t_subproblem_id);
    void add_constraint_and_variables_to_master(const Ctr& t_ctr);
    void add_variable_to_master(const Var& t_var);
    void add_objective_to_master();
    std::pair<Expr<Var, Var>, std::vector<Constant>> decompose_master_expression(const LinExpr<Var>& t_expr);
    void add_convexity_constraints();
public:
    DantzigWolfe(const Model& t_original_model, Annotation<Ctr, unsigned int> t_complicating_constraint_annotation);

    void build() override;

    Decomposition<Ctr>& decomposition() { return m_decomposition.value(); }

    [[nodiscard]] const Decomposition<Ctr>& decomposition() const { return m_decomposition.value(); }

    Model &model() override { return m_decomposition->master_problem(); }

    [[nodiscard]] const Model &model() const override { return m_decomposition->master_problem(); }
};

#endif //IDOL_RELAXATIONS_DANTZIGWOLFE_H
