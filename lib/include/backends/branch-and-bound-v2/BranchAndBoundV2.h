//
// Created by henri on 13/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDV2_H
#define IDOL_BRANCHANDBOUNDV2_H

#include "backends/branch-and-bound/NodeSet.h"
#include "BranchingRuleFactory.h"
#include "RelaxationBuilderFactory.h"
#include "NodeSelectionRuleFactory.h"

#include <memory>

template<class NodeT>
class BranchAndBoundV2 : public Algorithm {
    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;

    NodeSet<NodeT> m_active_nodes;
public:
    explicit BranchAndBoundV2(const AbstractModel& t_model,
                              const OptimizerFactory& t_node_optimizer,
                              const RelaxationBuilderFactory& t_relaxation_builder_factory,
                              const BranchingRuleFactory<NodeT>& t_branching_rule_factory,
                              const NodeSelectionRuleFactory<NodeT>& t_node_selection_rule_factory);
protected:
    void hook_optimize() override;
    void initialize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;
};

template<class NodeT>
BranchAndBoundV2<NodeT>::BranchAndBoundV2(const AbstractModel &t_model,
                                          const OptimizerFactory& t_node_optimizer,
                                          const RelaxationBuilderFactory& t_relaxation_builder_factory,
                                          const BranchingRuleFactory<NodeT>& t_branching_rule_factory,
                                          const NodeSelectionRuleFactory<NodeT>& t_node_selection_rule_factory)
    : Algorithm(t_model),
      m_relaxation_optimizer_factory(t_node_optimizer.clone()) {

}

template<class NodeT>
void BranchAndBoundV2<NodeT>::hook_optimize() {

}

template<class NodeT>
void BranchAndBoundV2<NodeT>::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

template<class NodeT>
void BranchAndBoundV2<NodeT>::update() {
    throw Exception("Not implemented update");
}

template<class NodeT>
void BranchAndBoundV2<NodeT>::remove(const Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

template<class NodeT>
void BranchAndBoundV2<NodeT>::remove(const Var &t_var) {
    throw Exception("Not implemented remove");
}

template<class NodeT>
void BranchAndBoundV2<NodeT>::add(const Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

template<class NodeT>
void BranchAndBoundV2<NodeT>::add(const Var &t_var) {
    throw Exception("Not implemented add");
}

template<class NodeT>
void BranchAndBoundV2<NodeT>::initialize() {

}

#endif //IDOL_BRANCHANDBOUNDV2_H
