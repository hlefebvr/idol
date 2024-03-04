//
// Created by henri on 22/03/23.
//

#ifndef IDOL_BESTESTIMATE_IMPL_H
#define IDOL_BESTESTIMATE_IMPL_H

#include "NodeSelectionRule.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/nodes/NodeSet.h"
#include "idol/errors/Exception.h"
#include <optional>

namespace idol::NodeSelectionRules {
    template<class NodeT> class BestEstimate;
}

template<class NodeT>
class idol::NodeSelectionRules::BestEstimate : public NodeSelectionRule<NodeT> {
    std::optional<double> m_root_node_obj;
    std::optional<double> m_root_node_sum_of_infeasibilities;

    double compute_score(const Node<NodeT>& t_node) {

        const double node_obj = t_node.info().objective_value();
        const double node_sum_of_infeasibilities = t_node.info().sum_of_infeasibilities();
        const double incumbent_obj = this->parent().incumbent().info().objective_value();

        return node_obj - ( m_root_node_obj.value() - incumbent_obj ) * node_sum_of_infeasibilities / m_root_node_sum_of_infeasibilities.value();
    }
public:
    explicit BestEstimate(Optimizers::BranchAndBound<NodeT>& t_parent) : NodeSelectionRule<NodeT>(t_parent) {}

    typename NodeSet<Node<NodeT>>::const_iterator operator()(const NodeSet<Node<NodeT>>& t_active_nodes) override {

        if (t_active_nodes.size() == 1 && t_active_nodes.by_objective_value().begin()->id() == 0) {
            const auto& root_node_it = t_active_nodes.by_objective_value().begin();
            m_root_node_obj = (double) root_node_it->info().objective_value();
            m_root_node_sum_of_infeasibilities = root_node_it->info().sum_of_infeasibilities();
            return root_node_it;
        }

        if (!this->parent().has_incumbent()) {
            return t_active_nodes.by_objective_value().begin();
        }

        double max = std::numeric_limits<double>::lowest();
        typename NodeSet<Node<NodeT>>::const_iterator argmax;
        for (auto it = t_active_nodes.by_objective_value().begin(),
                end = t_active_nodes.by_objective_value().end() ; it != end ; ++it) {

            const double score = compute_score(*it);

            if (score > max) {
                max = score;
                argmax = it;
            }

        }

        return argmax;
    }
};

#endif //IDOL_BESTESTIMATE_IMPL_H
