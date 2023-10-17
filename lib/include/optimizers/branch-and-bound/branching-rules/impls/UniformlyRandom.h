//
// Created by henri on 17.10.23.
//

#ifndef IDOL_IMPL_UNIFORMLYRANDOM_H
#define IDOL_IMPL_UNIFORMLYRANDOM_H

#include <random>
#include "VariableBranching.h"

namespace idol::BranchingRules {
    template<class NodeInfoT> class UniformlyRandom;
}

template<class NodeInfoT>
class idol::BranchingRules::UniformlyRandom : public VariableBranching<NodeInfoT> {
    unsigned int m_seed;
protected:
    std::list<std::pair<Var, double>> scoring_function(const std::list<Var> &t_var, const Solution::Primal &t_primal_values) override;
public:
    explicit UniformlyRandom(const Optimizers::BranchAndBound<NodeInfoT>& t_parent,
                             std::list<Var> t_branching_candidates,
                             unsigned int t_seed);
};

template<class NodeInfoT>
idol::BranchingRules::UniformlyRandom<NodeInfoT>::UniformlyRandom(
        const idol::Optimizers::BranchAndBound<NodeInfoT> &t_parent,
        std::list<Var> t_branching_candidates,
        unsigned int t_seed
        )
        : VariableBranching<NodeInfoT>(t_parent, std::move(t_branching_candidates)),
          m_seed(t_seed) {}

template<class NodeInfoT>
std::list<std::pair<idol::Var, double>>
idol::BranchingRules::UniformlyRandom<NodeInfoT>::scoring_function(const std::list<idol::Var> &t_variables,
                                                                  const idol::Solution::Primal &t_primal_values) {

    std::mt19937 engine(m_seed);
    std::uniform_real_distribution<double> distribution(0, 1);

    std::list<std::pair<Var, double>> result;

    for (const auto& var : t_variables) {
        result.emplace_back(var, distribution(engine));
    }

    return result;
}

#endif //IDOL_IMPL_UNIFORMLYRANDOM_H
