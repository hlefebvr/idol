//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_MOSTINFEASIBLE_H
#define IDOL_IMPL_MOSTINFEASIBLE_H

#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class NodeInfoT> class MostInfeasible;
}

template<class NodeInfoT>
class idol::BranchingRules::MostInfeasible : public VariableBranching<NodeInfoT> {
protected:
    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Solution::Primal &t_primal_values) override;
public:
    explicit MostInfeasible(const Optimizers::BranchAndBound<NodeInfoT>& t_parent, std::list<Var> t_branching_candidates);
};

template<class NodeInfoT>
idol::BranchingRules::MostInfeasible<NodeInfoT>::MostInfeasible(
        const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeInfoT>(t_parent, std::move(t_branching_candidates)) {}

template<class NodeInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::MostInfeasible<NodeInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                  const idol::Solution::Primal &t_primal_values) {

    std::list<std::pair<Var, double>> result;

    for (const auto& var : t_variables) {
        const double value = t_primal_values.get(var);
        const double score = std::abs(value - std::round(value));
        result.emplace_back(var, score);
    }

    return result;
}

#endif //IDOL_IMPL_MOSTINFEASIBLE_H
