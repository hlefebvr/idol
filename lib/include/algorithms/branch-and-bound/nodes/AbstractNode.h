//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTNODE_H
#define OPTIMIZE_ABSTRACTNODE_H

#include "solvers/Types.h"
#include <functional>

namespace Solution {
    class Primal;
    class Dual;
}

class Model;
class Var;
class Algorithm;

class AbstractNode {
    const unsigned int m_id;
public:
    explicit AbstractNode(unsigned int t_id) : m_id(t_id) {}

    virtual ~AbstractNode() = default;

    [[nodiscard]] virtual SolutionStatus status() const = 0;
    [[nodiscard]] virtual double objective_value() const = 0;
    [[nodiscard]] virtual const Solution::Primal& primal_solution() const = 0;
    [[nodiscard]] virtual const Solution::Dual& dual_solution() const = 0;
    [[nodiscard]] unsigned int id() const { return m_id; }
    
    virtual void save_solution(const Algorithm& t_strategy) = 0;
    [[nodiscard]] virtual AbstractNode* create_child(unsigned int t_id) const = 0;
    virtual void set_local_lower_bound(const Var& t_var, double t_lb) = 0;
    virtual void set_local_upper_bound(const Var& t_var, double t_ub) = 0;
};

template<>
class std::greater<AbstractNode*> {
public:
    bool operator()(const AbstractNode* t_a, const AbstractNode* t_b) const {
        return t_a->objective_value() > t_b->objective_value();
    }
};

#endif //OPTIMIZE_ABSTRACTNODE_H
