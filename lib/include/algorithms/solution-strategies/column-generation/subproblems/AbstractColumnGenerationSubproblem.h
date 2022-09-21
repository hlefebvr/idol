//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_ABSTRACTCOLUMNGENERATIONSUBPROBLEM_H
#define OPTIMIZE_ABSTRACTCOLUMNGENERATIONSUBPROBLEM_H

#include "algorithms/solution-strategies/AbstractSolutionStrategy.h"

class AbstractColumnGenerationSubproblem : public AbstractSolutionStrategy {
public:
    virtual void log_last_primal_solution() const = 0;

    virtual void save_last_primal_solution() = 0;

    [[nodiscard]] virtual Row get_pricing_objective(const Solution::Dual& t_duals) const = 0;

    virtual void update_pricing_objective(const Row& t_objective) = 0;

    [[nodiscard]] virtual bool is_unbounded() const = 0;

    [[nodiscard]] virtual bool is_infeasible() const = 0;

    [[nodiscard]] virtual bool could_not_be_solved_to_optimality() const = 0;

    [[nodiscard]] virtual bool improving_column_found() const = 0;

    virtual void add_column_to_rmp() = 0;
};

#endif //OPTIMIZE_ABSTRACTCOLUMNGENERATIONSUBPROBLEM_H
