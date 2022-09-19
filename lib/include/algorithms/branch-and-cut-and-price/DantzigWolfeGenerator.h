//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_DANTZIGWOLFEGENERATOR_H
#define OPTIMIZE_DANTZIGWOLFEGENERATOR_H

#include "ColumnGenerator.h"
#include <functional>

class DantzigWolfeGenerator : public ColumnGenerator {
    Ctr m_convexificiation_constraint;
    Map<Var, Var> m_rmp_to_subproblem_variables;

    Map<Var, Ctr> m_lower_bound_constraints; // SP Var -> LB Ctr
    Map<Var, Ctr> m_upper_bound_constraints; // SP Var -> UB Ctr

    Var get_subproblem_variable(const Var& t_rmp_variable) const;

   static  Expr expand(const Var& t_subproblem_variable, const ColumnGenerationSubProblem& t_subproblem);

    void set_lower_bound_rmp(const Var& t_rmp_variable, double t_lb, ColumnGenerationSubProblem& t_subproblem);

    void set_upper_bound_rmp(const Var& t_rmp_variable, double t_ub, ColumnGenerationSubProblem& t_subproblem);

    void set_bound_rmp(const Var& t_subproblem_variable, double t_bound, Map<Var, Ctr>& t_bound_constraints, const std::function<TempCtr(Expr&&, double)>& t_ctr_builder, const std::function<double(const Var&)>& t_get_bound, ColumnGenerationSubProblem& t_subproblem);
public:
    DantzigWolfeGenerator(const Model& t_rmp, const Model& t_subproblem, const Ctr& t_convexification_constraint);

    void set(const Var& t_rmp_var, const Var& t_sp_var);

    TempVar create_column(const Solution::Primal &t_primal_solution) const override;

    Row get_pricing_objective(const Solution::Dual &t_dual_solution) override;

    AbstractColumnGenerator *clone() const override;

    void set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

    void set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

    Solution::Primal primal_solution(const ColumnGenerationSubProblem &t_subproblem,
                                     const Solution::Primal &t_rmp_primals) const override;
};

#endif //OPTIMIZE_DANTZIGWOLFEGENERATOR_H
