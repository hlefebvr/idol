//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_ABSTRACTCOLUMNGENERATIONSUBPROBLEM_H
#define OPTIMIZE_ABSTRACTCOLUMNGENERATIONSUBPROBLEM_H

#include "modeling/solutions/Solution.h"
#include "modeling/constraints/TempCtr.h"
#include <optional>

class AbstractColumnGenerationSubproblem {
public:
    virtual ~AbstractColumnGenerationSubproblem() = default;

    virtual void build() = 0;

    virtual void solve() = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;

    [[nodiscard]] virtual Solution::Dual dual_solution() const = 0;

    virtual void log_last_primal_solution() const = 0;

    virtual void save_last_primal_solution() = 0;

    [[nodiscard]] virtual Row get_pricing_objective(const Solution::Dual& t_duals) const = 0;

    virtual void update_pricing_objective(const Row& t_objective) = 0;

    [[nodiscard]] virtual bool is_unbounded() const = 0;

    [[nodiscard]] virtual bool is_infeasible() const = 0;

    [[nodiscard]] virtual bool could_not_be_solved_to_optimality() const = 0;

    [[nodiscard]] virtual bool improving_column_found() const = 0;

    virtual void add_column_to_rmp() = 0;

    virtual bool set_lower_bound(const Var&, double t_lb) = 0;

    virtual bool set_upper_bound(const Var&, double t_ub) = 0;

    virtual std::optional<Ctr> contribute_to_add_constraint(TempCtr& t_temporay_constraint) = 0;

    virtual bool update_constraint_rhs(const Ctr& t_ctr, double t_rhs) = 0;

    virtual bool remove_constraint(const Ctr& t_ctr) = 0;
};

#endif //OPTIMIZE_ABSTRACTCOLUMNGENERATIONSUBPROBLEM_H
