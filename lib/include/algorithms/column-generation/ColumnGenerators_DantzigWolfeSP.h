//
// Created by henri on 20/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATORS_DANTZIGWOLFESP_H
#define OPTIMIZE_COLUMNGENERATORS_DANTZIGWOLFESP_H

#include "ColumnGenerators_Basic.h"
#include <functional>

namespace ColumnGenerators {
    class DantzigWolfeSP;
}

class ColumnGenerators::DantzigWolfeSP : public ColumnGenerators::Basic {
    Ctr m_convexificiation_constraint;
protected:
    static Expr<Var> expand(const Var &t_subproblem_variable, const ColumnGenerationSP &t_subproblem);

public:
    DantzigWolfeSP(Model &t_rmp, const Model &t_subproblem);

    TempVar create_column(const Solution::Primal &t_primal_solution) const override;

    Row get_pricing_objective(const Solution::Dual &t_dual_solution) override;
};

#endif //OPTIMIZE_COLUMNGENERATORS_DANTZIGWOLFESP_H
