//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_DANTZIGWOLFEGENERATOR_H
#define OPTIMIZE_DANTZIGWOLFEGENERATOR_H

#include "DantzigWolfeGeneratorSP.h"

class DantzigWolfeGenerator : public DantzigWolfeGeneratorSP {

    Map<Var, Ctr> m_lower_bound_constraints; // SP Var -> LB Ctr
    Map<Var, Ctr> m_upper_bound_constraints; // SP Var -> UB Ctr

    void set_lower_bound_rmp(const Var& t_rmp_variable, double t_lb, ColumnGenerationSubProblem& t_subproblem);

    void set_upper_bound_rmp(const Var& t_rmp_variable, double t_ub, ColumnGenerationSubProblem& t_subproblem);

    void set_bound_rmp(const Var& t_subproblem_variable, double t_bound, Map<Var, Ctr>& t_bound_constraints, const std::function<TempCtr(Expr&&, double)>& t_ctr_builder, const std::function<double(const Var&)>& t_get_bound, ColumnGenerationSubProblem& t_subproblem);
public:
    DantzigWolfeGenerator(Model& t_rmp, const Model& t_subproblem);

    AbstractColumnGenerator *clone() const override;

    void set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

    void set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

};

#endif //OPTIMIZE_DANTZIGWOLFEGENERATOR_H
