//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_DANTZIGWOLFE_SP_STRATEGY_H
#define OPTIMIZE_DANTZIGWOLFE_SP_STRATEGY_H

#include "ColumnGenerator.h"
#include <functional>

class DantzigWolfe_SP_Strategy : public ColumnGenerator {
    Ctr m_convexificiation_constraint;
protected:
    static  Expr expand(const Var& t_subproblem_variable, const ColumnGenerationSubProblem& t_subproblem);
public:
    DantzigWolfe_SP_Strategy(Model& t_rmp, const Model& t_subproblem);

    TempVar create_column(const Solution::Primal &t_primal_solution) const override;

    Row get_pricing_objective(const Solution::Dual &t_dual_solution) override;

    void set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

    void set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

};

#endif //OPTIMIZE_DANTZIGWOLFE_SP_STRATEGY_H
