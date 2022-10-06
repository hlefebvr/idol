//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_ABSTRACTCUTGENERATIONSUBPROBLEM_H
#define OPTIMIZE_ABSTRACTCUTGENERATIONSUBPROBLEM_H

#include "modeling/expressions/Row.h"

namespace Solution {
    class Primal;
}

class AbstractCutGenerationSubproblem {
public:
    virtual ~AbstractCutGenerationSubproblem() = default;

    virtual void build() = 0;

    virtual void solve() = 0;

    [[nodiscard]] virtual Row get_separation_objective(const Solution::Primal& t_primals) const = 0;

    virtual void update_separation_objective(const Row& t_objective) = 0;

    virtual void save_last_primal_solution() = 0;

    virtual void log_last_primal_solution() = 0;

    virtual bool violated_cut_found() = 0;

    virtual void add_cut_to_rmp() = 0;

    virtual bool set_lower_bound(const Var& t_var, double t_lb) = 0;

    virtual bool set_upper_bound(const Var& t_var, double t_ub) = 0;

    [[nodiscard]] virtual bool is_unbounded() const = 0;

    [[nodiscard]] virtual bool is_infeasible() const = 0;

    [[nodiscard]] virtual bool could_not_be_solved_to_optimality() const = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;
};

#endif //OPTIMIZE_ABSTRACTCUTGENERATIONSUBPROBLEM_H
