//
// Created by henri on 08.02.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
#define IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H

#include "idol/modeling/solutions/Solution.h"
#include "idol/modeling/matrix/Row.h"
#include "idol/modeling/constraints/Ctr.h"

namespace idol {
    namespace Robust {
        class ColumnAndConstraintGenerationSeparator;
    }

    namespace Optimizers::Robust {
        class ColumnAndConstraintGeneration;
    }

}

class idol::Robust::ColumnAndConstraintGenerationSeparator {
protected:
    [[nodiscard]] static Expr<Var, Var> fix_and_revert(const LinExpr<Var>& t_expr, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) ;
    [[nodiscard]] static Expr<Var, Var> revert(const Constant& t_constant, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent) ;
public:
    virtual ~ColumnAndConstraintGenerationSeparator() = default;

    [[nodiscard]] virtual ColumnAndConstraintGenerationSeparator* clone() const = 0;

    virtual Solution::Primal operator()(const Optimizers::Robust::ColumnAndConstraintGeneration& t_parent,
                                        const Solution::Primal& t_upper_level_solution,
                                        const Row& t_row,
                                        CtrType t_type) const = 0;
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
