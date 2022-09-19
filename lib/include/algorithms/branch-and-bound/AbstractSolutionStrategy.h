//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H
#define OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H

#include "../../solvers/solutions/Solution.h"
#include "../../modeling/variables/TempVar.h"

class AbstractNode;
class Solver;
class Var;

class AbstractSolutionStrategy {
    static void throw_not_implemented(const std::string& t_functionality, const std::string& t_method_to_implement) {
        throw std::runtime_error(t_functionality + " is not implemented. "
                                 "If you wish to implement it, "
                                 "please override the " + t_method_to_implement + " method.");
    }
public:
    virtual ~AbstractSolutionStrategy() = default;

    virtual void build() {}

    virtual void solve() = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;

    [[nodiscard]] virtual Solution::Dual dual_solution() const {
        throw_not_implemented("Retrieving dual solution", "dual_solution");
        return {};
    }

    [[nodiscard]] virtual Solution::Dual farkas_certificate() const {
        throw_not_implemented("Retrieving Farkas certificate", "farkas_certificate");
        return {};
    }

    virtual void set_lower_bound(const Var& t_var, double t_lb) {
        throw_not_implemented("Updating variable LB", "set_lower_bound");
    }

    virtual void set_upper_bound(const Var& t_var, double t_ub) {
        throw_not_implemented("Updating variable UB", "set_upper_bound");
    }

    virtual void set_objective(const Row& t_objective) {
        throw_not_implemented("Updating objective", "set_objective");
    }

    virtual Var add_column(TempVar t_temporary_variable) {
        throw std::runtime_error("Adding columns is not implemented. "
                               "If you wish to implement it, "
                               "please override the add_column method.");
    }

    virtual void remove_variable(const Var& t_variable) {
        throw_not_implemented("Removing variable", "remove_variable");
    }
};

#endif //OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H
