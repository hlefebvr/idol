//
// Created by henri on 08.02.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
#define IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H

#include "idol/modeling/solutions/Solution.h"
#include "idol/modeling/matrix/Row.h"
#include "idol/modeling/constraints/Ctr.h"

namespace idol {
    class ColumnAndConstraintGenerationSeparator;

    namespace Optimizers {
        class ColumnAndConstraintGeneration;
    }

}

class idol::ColumnAndConstraintGenerationSeparator {
protected:
    [[nodiscard]] static Expr<Var, Var> fix_and_revert(const LinExpr<Var>& t_expr, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) ;
    [[nodiscard]] static Expr<Var, Var> revert(const Constant& t_constant, const Optimizers::ColumnAndConstraintGeneration &t_parent) ;
public:
    virtual ~ColumnAndConstraintGenerationSeparator() = default;

    [[nodiscard]] virtual ColumnAndConstraintGenerationSeparator* clone() const = 0;

    virtual Solution::Primal operator()(const Optimizers::ColumnAndConstraintGeneration& t_parent,
                                        const Solution::Primal& t_upper_level_solution,
                                        const Row& t_row,
                                        CtrType t_type) const = 0;
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
