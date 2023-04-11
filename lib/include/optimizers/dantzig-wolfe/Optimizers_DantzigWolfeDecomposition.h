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
    std::unique_ptr<OptimizerFactory> m_pricing_optimizer;

    Annotation<Ctr, unsigned int> m_ctr_annotation;
    Annotation<Var, unsigned int> m_var_annotation;

    bool m_branching_on_master = true;

    Map<Var, Ctr> m_lower_bound_constraints;
    Map<Var, Ctr> m_upper_bound_constraints;
public:
    DantzigWolfeDecomposition(const Model& t_original_formulation,
                              const Annotation<Ctr, unsigned int>& t_constraint_flag,
                              const Annotation<Var, unsigned int>& t_variable_flag,
                              Model *t_master_problem,
                              const std::vector<Model *> &t_subproblems,
                              std::vector<Column> t_generation_patterns,
                              const OptimizerFactory& t_pricing_optimizer);

    [[nodiscard]] std::string name() const override { return "dantzig-wolfe"; }

    virtual void set_branching_on_master(bool t_value) { m_branching_on_master = t_value; }

    [[nodiscard]] bool branching_on_master() const { return m_branching_on_master; }

    [[nodiscard]] const Annotation<Ctr, unsigned int>& ctr_annotation() const { return m_ctr_annotation; };

    [[nodiscard]] const Annotation<Var, unsigned int>& var_annotation() const { return m_var_annotation; };
protected:
    void set_objective(Expr<Var, Var> &&t_objective) override;

    [[nodiscard]] double get_var_primal(const Var &t_var) const override;

    void update_var_lb(const Var &t_var) override;
    void update_var_ub(const Var &t_var) override;

    [[nodiscard]] double get_subproblem_primal_value(const Var& t_var, unsigned int t_subproblem_id) const;
    void set_subproblem_lower_bound(const Var& t_var, unsigned int t_subproblem_id, double t_value);
    void set_subproblem_upper_bound(const Var& t_var, unsigned int t_subproblem_id, double t_value);
    void apply_subproblem_bound_on_master(bool t_is_lb, const Var &t_var, unsigned int t_subproblem_id, double t_value);

    LinExpr<Var> expand_subproblem_variable(const Var& t_var, unsigned int t_subproblem_id);

    void add(const Var &t_var) override;

    void add(const Ctr &t_ctr) override;
};

#endif //IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
