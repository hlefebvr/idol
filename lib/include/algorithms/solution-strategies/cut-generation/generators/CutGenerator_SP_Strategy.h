//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATOR_SP_STRATEGY_H
#define OPTIMIZE_CUTGENERATOR_SP_STRATEGY_H

#include "AbstractCutGenerator.h"
#include "algorithms/solution-strategies/decomposition/generators/BaseGenerator.h"

class Model;

class CutGenerator_SP_Strategy : public AbstractCutGenerator, public BaseGenerator<Var> {
protected:
    static void remove_columns_violating_lower_bound(const Var& t_cut, double t_cut_primal_solution, CutGenerationSubproblem& t_subproblem);

    static void remove_columns_violating_upper_bound(const Var& t_cut, double t_cut_primal_solution, CutGenerationSubproblem& t_subproblem);
public:
    CutGenerator_SP_Strategy(const Model& t_rmp_model, const Model& t_sp_model);

    Row get_separation_objective(const Solution::Primal &t_primals) override;

    TempCtr create_cut(const Solution::Primal &t_primals) const override;

    bool set_lower_bound(const Var &t_var, double t_lb, CutGenerationSubproblem &t_subproblem) override;

    bool set_upper_bound(const Var &t_var, double t_ub, CutGenerationSubproblem &t_subproblem) override;

    Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Solution::Dual &t_rmp_duals) const override;
};

#endif //OPTIMIZE_CUTGENERATOR_SP_STRATEGY_H
