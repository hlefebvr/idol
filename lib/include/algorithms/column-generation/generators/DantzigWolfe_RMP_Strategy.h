//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_DANTZIGWOLFE_RMP_STRATEGY_H
#define OPTIMIZE_DANTZIGWOLFE_RMP_STRATEGY_H

#include "DantzigWolfe_SP_Strategy.h"

class DantzigWolfe_RMP_Strategy : public DantzigWolfe_SP_Strategy {

    Map<Var, Ctr> m_lower_bound_constraints; // SP Var -> LB Ctr
    Map<Var, Ctr> m_upper_bound_constraints; // SP Var -> UB Ctr

    void set_lower_bound_rmp(const Var& t_rmp_variable, double t_lb, ColumnGenerationSubproblem& t_subproblem);

    void set_upper_bound_rmp(const Var& t_rmp_variable, double t_ub, ColumnGenerationSubproblem& t_subproblem);

    void set_bound_rmp(const Var& t_subproblem_variable, double t_bound, Map<Var, Ctr>& t_bound_constraints, const std::function<TempCtr(Expr<Var>&&, double)>& t_ctr_builder, const std::function<double(const Var&)>& t_get_bound, ColumnGenerationSubproblem& t_subproblem);
public:
    DantzigWolfe_RMP_Strategy(Model& t_rmp, const Model& t_subproblem);

    bool set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubproblem &t_subproblem) override;

    bool set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSubproblem &t_subproblem) override;

    std::optional<Ctr>
    contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSubproblem &t_subproblem) override;

    bool update_constraint_rhs(const Ctr &t_ctr, double t_rhs, ColumnGenerationSubproblem &t_subproblem) override;

    bool remove_constraint(const Ctr &t_ctr, ColumnGenerationSubproblem &t_subproblem) override;
};

#endif //OPTIMIZE_DANTZIGWOLFE_RMP_STRATEGY_H
