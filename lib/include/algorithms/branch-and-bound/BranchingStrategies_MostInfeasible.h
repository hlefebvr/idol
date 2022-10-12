//
// Created by henri on 21/09/22.
//

#ifndef OPTIMIZE_BRANCHINGSTRATEGIES_MOSTINFEASIBLE_H
#define OPTIMIZE_BRANCHINGSTRATEGIES_MOSTINFEASIBLE_H

#include "BranchingStrategy.h"
#include "../../modeling/variables/Variable.h"
#include "../../modeling/numericals.h"
#include "../../errors/Exception.h"
#include "../logs/Log.h"
#include <vector>
#include <list>
#include <functional>

namespace BranchingStrategies {
    class MostInfeasible;
}

class BranchingStrategies::MostInfeasible {
public:
    template<class NodeT> class Strategy;
};

template<class NodeT>
class BranchingStrategies::MostInfeasible::Strategy : public BranchingStrategyWithNodeType<NodeT> {
    std::vector<Var> m_branching_candidates;
protected:
    static double fractional_part(double t_x);
    static bool is_integer(double t_x);
    virtual double score(const Var& t_var, const NodeT& t_node) const;
public:
    explicit Strategy(std::vector<Var> t_branching_candidates) : m_branching_candidates(std::move(t_branching_candidates)) {}

    bool is_valid(const NodeT &t_node) const override;

    std::list<NodeT *> create_child_nodes(const NodeT &t_node, const std::function<unsigned int()>& t_id_provider) const override;
};

template<class NodeT>
bool BranchingStrategies::MostInfeasible::Strategy<NodeT>::is_valid(const NodeT &t_node) const {
    const auto& primal = t_node.primal_solution();

    for (const auto& var : m_branching_candidates) {
        if (double value = primal.get(var) ; !is_integer(value)) {
            EASY_LOG(Trace, "branch-and-bound", "Node " << t_node.id() << " solution not valid (value = " << value << ")." );
            return false;
        }
    }

    return true;
}

template<class NodeT>
std::list<NodeT *> BranchingStrategies::MostInfeasible::Strategy<NodeT>::create_child_nodes(const NodeT &t_node, const std::function<unsigned int()>& t_id_provider) const {

    double max_infeas = 0.;
    const Var* selected_var = nullptr;

    for (const auto& var : m_branching_candidates) {
        if (double infeas = score(var, t_node) ; infeas > max_infeas) {
            max_infeas = infeas;
            selected_var = &var;
        }
    }

    if (!selected_var || max_infeas <= ToleranceForIntegrality) {
        throw Exception("Maximum infeasibility is less than ToleranceForIntegrality");
    }

    const double value = t_node.primal_solution().get(*selected_var);

    auto* n1 = t_node.create_child( t_id_provider() );
    n1->set_local_lower_bound(*selected_var, std::ceil(value));

    auto* n2 = t_node.create_child( t_id_provider() );
    n2->set_local_upper_bound(*selected_var, std::floor(value));

    return { n1, n2 };
}

template<class NodeT>
double BranchingStrategies::MostInfeasible::Strategy<NodeT>::fractional_part(double t_x) {
    return std::abs(std::round(t_x) - t_x);
}

template<class NodeT>
bool BranchingStrategies::MostInfeasible::Strategy<NodeT>::is_integer(double t_x) {
    return fractional_part(t_x) <= ToleranceForIntegrality;
}

template<class NodeT>
double BranchingStrategies::MostInfeasible::Strategy<NodeT>::score(const Var &t_var, const NodeT &t_node) const {
    const double frac_value = fractional_part(t_node.primal_solution().get(t_var));
    if (frac_value <= ToleranceForIntegrality) {
        return -Inf;
    }
    return std::max(frac_value, 1. - frac_value);
}

#endif //OPTIMIZE_BRANCHINGSTRATEGIES_MOSTINFEASIBLE_H
