//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_FIRSTINFEASIBLEFOUND_H
#define IDOL_IMPL_FIRSTINFEASIBLEFOUND_H

#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class NodeInfoT> class FirstInfeasibleFound;
}

template<class NodeInfoT>
class idol::BranchingRules::FirstInfeasibleFound : public VariableBranching<NodeInfoT> {
protected:
    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Solution::Primal &t_primal_values) override;
public:
    explicit FirstInfeasibleFound(const Optimizers::BranchAndBound<NodeInfoT>& t_parent, std::list<Var> t_branching_candidates);
};

template<class NodeInfoT>
idol::BranchingRules::FirstInfeasibleFound<NodeInfoT>::FirstInfeasibleFound(
        const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeInfoT>(t_parent, std::move(t_branching_candidates)) {}

template<class NodeInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::FirstInfeasibleFound<NodeInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                  const idol::Solution::Primal &t_primal_values) {
    return { { t_variables.front(), -1. } };
}

#endif //IDOL_IMPL_FIRSTINFEASIBLEFOUND_H
