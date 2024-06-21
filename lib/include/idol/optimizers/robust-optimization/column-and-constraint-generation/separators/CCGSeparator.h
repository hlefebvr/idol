//
// Created by henri on 08.02.24.
//

#ifndef IDOL_ROBUST_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
#define IDOL_ROBUST_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H

#include "../../../../modeling/solutions/Solution.h"
#include "../../../../modeling/matrix/Row.h"
#include "../../../../modeling/constraints/Ctr.h"

namespace idol {
    namespace Robust {
        class CCGSeparator;
    }

    namespace Optimizers::Robust {
        class ColumnAndConstraintGeneration;
    }

}

class idol::Robust::CCGSeparator {
protected:
    [[nodiscard]] static Expr<Var, Var> fix_and_revert(const LinExpr<Var>& t_expr, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) ;
    [[nodiscard]] static Expr<Var, Var> revert(const Constant& t_constant, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent) ;
public:
    virtual ~CCGSeparator() = default;

    [[nodiscard]] virtual CCGSeparator* clone() const = 0;

    virtual Solution::Primal solve_feasibility_separation_problem(const Optimizers::Robust::ColumnAndConstraintGeneration& t_parent,
                                                                  const Solution::Primal& t_upper_level_solution) const = 0;

    virtual Solution::Primal solve_separation_problem(const Optimizers::Robust::ColumnAndConstraintGeneration& t_parent,
                                                      const Solution::Primal& t_upper_level_solution,
                                                      const Row& t_row,
                                                      CtrType t_type) const = 0;
};

#endif //IDOL_ROBUST_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
