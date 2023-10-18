//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_FIRSTINFEASIBLEFOUND_H
#define IDOL_IMPL_FIRSTINFEASIBLEFOUND_H

#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class NodeVarInfoT> class FirstInfeasibleFound;
}

template<class NodeVarInfoT>
class idol::BranchingRules::FirstInfeasibleFound : public VariableBranching<NodeVarInfoT> {
protected:
    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Node<NodeVarInfoT> &t_node) override;
public:
    explicit FirstInfeasibleFound(const Optimizers::BranchAndBound<NodeVarInfoT>& t_parent, std::list<Var> t_branching_candidates);
};

template<class NodeVarInfoT>
idol::BranchingRules::FirstInfeasibleFound<NodeVarInfoT>::FirstInfeasibleFound(
        const idol::Optimizers::BranchAndBound<NodeVarInfoT> &t_parent, std::list<Var> t_branching_candidates)
        : VariableBranching<NodeVarInfoT>(t_parent, std::move(t_branching_candidates)) {}

template<class NodeVarInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::FirstInfeasibleFound<NodeVarInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                        const Node<NodeVarInfoT> &t_node) {
    return { { t_variables.front(), -1. } };
}

#endif //IDOL_IMPL_FIRSTINFEASIBLEFOUND_H
