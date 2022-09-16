//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_ABSTRACTGENERATIONSTRATEGY_H
#define OPTIMIZE_ABSTRACTGENERATIONSTRATEGY_H

#include "../branch-and-bound/AbstractSolutionStrategy.h"
#include "DecompositionId.h"

class AbstractGenerationStrategy : public AbstractSolutionStrategy {
    DecompositionId m_id;
protected:
    [[nodiscard]] unsigned int rmp_id() const;

    void rmp_solve();

    [[nodiscard]] Solution::Primal rmp_primal_solution() const;

    [[nodiscard]] Solution::Dual rmp_dual_solution() const;

    [[nodiscard]] Solution::Dual rmp_farkas_certificate() const;

    void rmp_add_column(TempVar t_temporary_variable);
public:
    explicit AbstractGenerationStrategy(DecompositionId&& t_decomposition_id);
};

#endif //OPTIMIZE_ABSTRACTGENERATIONSTRATEGY_H
