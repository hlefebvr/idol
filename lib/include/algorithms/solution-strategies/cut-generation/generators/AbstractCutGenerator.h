//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_ABSTRACTCUTGENERATOR_H
#define OPTIMIZE_ABSTRACTCUTGENERATOR_H

#include <modeling/expressions/Row.h>
#include <modeling/constraints/TempCtr.h>

namespace Solution {
    class Primal;
}

class AbstractCutGenerator {
    CtrType m_type = GreaterOrEqual;
public:
    virtual ~AbstractCutGenerator() = default;

    virtual Row get_separation_objective(const Solution::Primal& t_primals) = 0;

    [[nodiscard]] virtual TempCtr create_cut(const Solution::Primal& t_primals) const = 0;

    CtrType type() const { return m_type; }
};

#endif //OPTIMIZE_ABSTRACTCUTGENERATOR_H
