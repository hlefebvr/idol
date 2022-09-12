//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLUTION_H
#define OPTIMIZE_SOLUTION_H

#include "containers/Map.h"
#include "containers/Optional.h"
#include "containers/IteratorForward.h"
#include "modeling/variables/Variable.h"
#include "modeling/constraints/Constraint.h"
#include "solvers/Types.h"
#include "modeling/numericals.h"

class Solution {
    SolutionStatus m_status = Unknown;
    Optional<double> m_objective_value;
    Optional<Map<Var, double>> m_primal_values;
    Optional<Map<Var, double>> m_reduced_costs;
    Optional<Map<Ctr, double>> m_dual_values;

    template<class T> void set(Optional<Map<T, double>>& t_container, const T& t_key, double t_value);
    template<class T> double get(const Optional<Map<T, double>>& t_container, const T& t_key) const;
public:
    Solution() = default;

    Solution(const Solution&) = default;
    Solution(Solution&&) noexcept = default;

    Solution& operator=(const Solution&) = default;
    Solution& operator=(Solution&&) noexcept = default;

    explicit Solution(SolutionStatus t_status);

    void set_status(SolutionStatus t_status);

    SolutionStatus status() const;

    void set_value(double t_value);

    double value() const;

    bool has_value() const;

    void reset_objective_value();

    void set_primal_value(const Var& t_var, double t_value);

    double value(const Var& t_var) const;

    bool has_primal_values() const;

    void reset_primal_values();

    void set_reduced_cost(const Var& t_var, double t_value);

    bool has_reduced_costs() const;

    double reduced_cost(const Var& t_var) const;

    void reset_reduced_costs();

    void set_dual_value(const Ctr& t_ctr, double t_value);

    double value(const Ctr& t_ctr) const;

    bool has_dual_values() const;

    void reset_dual_values();

    using PrimalValues = ConstIteratorForward<Map<Var, double>>;
    using ReducedCosts = ConstIteratorForward<Map<Var, double>>;
    using DualValues = ConstIteratorForward<Map<Ctr, double>>;

    PrimalValues primal_values() const;

    ReducedCosts reduced_costs() const;

    DualValues dual_values() const;

    void reset();
};

template<class T>
void Solution::set(Optional<Map<T, double>> &t_container, const T &t_key, double t_value) {

    if (!t_container) {
        t_container.template emplace();
    }

    if (equals(t_value, 0., ToleranceForSparsity)) {
        t_container->erase(t_key);
        return;
    }

    auto [it, success] = t_container->template emplace(t_key, t_value);
    if (!success) {
        it->second = t_value;
    }

}

template<class T>
double Solution::get(const Optional<Map<T, double>> &t_container, const T &t_key) const {
    if (!t_container) {
        throw std::runtime_error("No solution found.");
    }
    auto it = t_container->find(t_key);
    return it == t_container->end() ? 0. : it->second;
}

#endif //OPTIMIZE_SOLUTION_H
