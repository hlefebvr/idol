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
public:
    virtual ~ColumnAndConstraintGenerationSeparator() = default;

    [[nodiscard]] virtual ColumnAndConstraintGenerationSeparator* clone() const = 0;

    virtual Solution::Primal operator()(const Optimizers::ColumnAndConstraintGeneration& t_parent,
                                        const Solution::Primal& t_upper_level_solution,
                                        const Row& t_row,
                                        CtrType t_type) const = 0;
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
