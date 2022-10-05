//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_LAZYCUTS_H
#define OPTIMIZE_LAZYCUTS_H

#include "AbstractCutGenerator.h"
#include "algorithms/solution-strategies/decomposition/generators/BaseGenerator.h"

class Model;

class LazyCuts : public AbstractCutGenerator, public BaseGenerator<Var> {
public:
    LazyCuts(const Model& t_rmp_model, const Model& t_sp_model);

    Row get_separation_objective(const Solution::Primal &t_primals) override;

    TempCtr create_cut(const Solution::Primal &t_primals) const override;
};

#endif //OPTIMIZE_LAZYCUTS_H
