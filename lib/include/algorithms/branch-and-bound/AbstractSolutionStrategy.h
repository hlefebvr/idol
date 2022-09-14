//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H
#define OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H

class Node;
class Solver;
class Var;

class AbstractSolutionStrategy {
public:
    virtual ~AbstractSolutionStrategy() = default;
    virtual void initialize() = 0;
    virtual void solve() const = 0;

    [[nodiscard]] virtual const Solver& solver() const = 0;
    virtual void reset_local_changes() = 0;

    virtual void set_local_lower_bound(const Var& t_var, double t_lb) = 0;
    virtual void set_local_upper_bound(const Var& t_var, double t_ub) = 0;
};

#endif //OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H
