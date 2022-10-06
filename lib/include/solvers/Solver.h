//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_SOLVER_H
#define OPTIMIZE_SOLVER_H

#include "modeling/solutions/Solution.h"
#include <string>

class Solver {
public:
    virtual ~Solver() = default;

    virtual void write(const std::string& t_filename) = 0;

    virtual void solve() = 0;

    virtual void set_infeasible_or_unbounded_info(bool t_value) = 0;

    [[nodiscard]] virtual bool infeasible_or_unbounded_info() const = 0;

    virtual void set_algorithm_for_lp(AlgorithmForLP t_algorithm) = 0;

    virtual void set_presolve(bool t_value) = 0;

    [[nodiscard]] virtual bool presolve() const = 0;

    [[nodiscard]] virtual AlgorithmForLP algorithm_for_lp() const = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;

    [[nodiscard]] virtual Solution::Primal unbounded_ray() const = 0;

    [[nodiscard]] virtual Solution::Dual dual_solution() const = 0;

    [[nodiscard]] virtual Solution::Dual dual_farkas() const = 0;

    [[nodiscard]] virtual Solution::Dual iis() const = 0;

    virtual void compute_iis() = 0;
};

#endif //OPTIMIZE_SOLVER_H
