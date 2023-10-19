//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_LEASTINFEASIBLE_H
#define IDOL_IMPL_LEASTINFEASIBLE_H

#include <cmath>
#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class NodeVarInfoT> class LeastInfeasible;
}

template<class NodeVarInfoT>
class idol::BranchingRules::LeastInfeasible : public VariableBranching<NodeVarInfoT> {
protected:
    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Node<NodeVarInfoT> &t_node) override;
public:
    explicit LeastInfeasible(const Optimizers::BranchAndBound<NodeVarInfoT>& t_parent, std::list<Var> t_branching_candidates);
};

template<class NodeVarInfoT>
idol::BranchingRules::LeastInfeasible<NodeVarInfoT>::LeastInfeasible(
        const idol::Optimizers::BranchAndBound<NodeVarInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeVarInfoT>(t_parent, std::move(t_branching_candidates)) {}

template<class NodeVarInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::LeastInfeasible<NodeVarInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                   const Node<NodeVarInfoT> &t_node) {

    const auto& primal_solution = t_node.info().primal_solution();

    std::list<std::pair<Var, double>> result;

    for (const auto& var : t_variables) {
        const double value = primal_solution.get(var);
        const double score = -std::abs(value - std::round(value));
        result.emplace_back(var, score);
    }

    return result;
}

#endif //IDOL_IMPL_LEASTINFEASIBLE_H
