//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_SOLUTION_H
#define OPTIMIZE_SOLUTION_H

#include "idol/modeling/variables/Var.h"
#include "idol/modeling/constraints/Ctr.h"
#include "AbstractSolution.h"

namespace idol::Solution {

    /**
     * Primal solution class.
     *
     * This class is used to store a primal solution to an optimization problem.
     *
     * Typically, it is obtained by calling the `save_primal` function, or by creating a new solution from scratch.
     *
     * **Example 1**:
     * ```cpp
     * model.optimize();
     * if (model.get_status() == Optimal) {
     *      Solution::Primal solution = save_primal(model);
     *      std::cout << solution << std::endl;
     * }
     * ```
     *
     * **Example 2**:
     * ```cpp
     * Solution::Primal solution;
     *
     * solution.set_status(Feasible);
     * solution.set_objective_value(13.);
     * solution.set(x, 1.);
     * ```
     */
    class Primal : public AbstractSolution<Var, Primal> {};

    /**
     * Dual solution class.
     *
     * This class is used to store a dual solution to an optimization problem.
     *
     * Typically, it is obtained by calling the `save_dual` function, or by creating a new solution from scratch.
     *
     * **Example 1**:
     * ```cpp
     * model.optimize();
     * if (model.get_status() == Optimal) {
     *      Solution::Dual solution = save_dual(model);
     *      std::cout << solution << std::endl;
     * }
     * ```
     *
     * **Example 2**:
     * ```cpp
     * Solution::Dual solution;
     *
     * solution.set_status(Feasible);
     * solution.set_objective_value(13.);
     * solution.set(constraint_1, -100.);
     * ```
     */
    class Dual : public AbstractSolution<Ctr, Dual> {};


    static Solution::Primal operator+(Solution::Primal t_a, const Solution::Primal& t_b) {
        return t_a += t_b;
    }

    static Solution::Dual operator+(Solution::Dual t_a, const Solution::Dual& t_b) {
        return t_a += t_b;
    }

    static Solution::Primal operator*(double t_factor, Solution::Primal t_solution) {
        return t_solution *= t_factor;
    }

    static Solution::Dual operator*(double t_factor, Solution::Dual t_solution) {
        return t_solution *= t_factor;
    }

}

#endif //OPTIMIZE_SOLUTION_H
