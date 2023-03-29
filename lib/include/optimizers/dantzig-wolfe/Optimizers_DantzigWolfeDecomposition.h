//
// Created by henri on 24/03/23.
//

#ifndef IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
#define IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H

#include "optimizers/column-generation/Optimizers_ColumnGeneration.h"

namespace Optimizers {
    class DantzigWolfeDecomposition;
}

class Optimizers::DantzigWolfeDecomposition : public Optimizers::ColumnGeneration {
    Annotation<Ctr, unsigned int> m_constraint_flag;
    Annotation<Var, unsigned int> m_variable_flag;

    Map<Var, Ctr> m_lower_bound_constraints;
    Map<Var, Ctr> m_upper_bound_constraints;
public:
    DantzigWolfeDecomposition(const Model& t_original_formulation,
                              const Annotation<Ctr, unsigned int>& t_constraint_flag,
                              const Annotation<Var, unsigned int>& t_variable_flag,
                              Model *t_master_problem,
                              const std::vector<Model *> &t_subproblems,
                              std::vector<Column> t_generation_patterns);

    [[nodiscard]] std::string name() const override { return "dantzig-wolfe"; }
protected:
    void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_expr) override;
    [[nodiscard]] double get(const Req<double, Var> &t_attr, const Var &t_var) const override;
    void set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) override;

    [[nodiscard]] double get_subproblem_primal_value(const Var& t_var, unsigned int t_subproblem_id) const;
    void set_subproblem_lower_bound(const Var& t_var, unsigned int t_subproblem_id, double t_value);
    void set_subproblem_upper_bound(const Var& t_var, unsigned int t_subproblem_id, double t_value);
    void apply_subproblem_bound_on_master(const Req<double, Var>& t_attr, const Var &t_var, unsigned int t_subproblem_id, double t_value);

    LinExpr<Var> expand_subproblem_variable(const Var& t_var, unsigned int t_subproblem_id);
};

#endif //IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
