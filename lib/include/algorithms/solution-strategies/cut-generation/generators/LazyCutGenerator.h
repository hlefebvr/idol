//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_LAZYCUTGENERATOR_H
#define OPTIMIZE_LAZYCUTGENERATOR_H

#include "AbstractCutGenerator.h"
#include "algorithms/solution-strategies/decomposition/generators/BaseGenerator.h"

class Model;

class LazyCutGenerator : public AbstractCutGenerator, public BaseGenerator<Var> {
public:
    LazyCutGenerator(const Model& t_rmp_model, const Model& t_sp_model);

    Row get_separation_objective(const Solution::Primal &t_primals) override;

    TempCtr create_cut(const Solution::Primal &t_primals) const override;

    bool set_lower_bound(const Var &t_var, double t_lb, CutGenerationSubproblem &t_subproblem) override;

    bool set_upper_bound(const Var &t_var, double t_lb, CutGenerationSubproblem &t_subproblem) override;
};

#endif //OPTIMIZE_LAZYCUTGENERATOR_H
