//
// Created by henri on 13/03/23.
//

#ifndef IDOL_BRANCHANDBOUNDV2_H
#define IDOL_BRANCHANDBOUNDV2_H

#include "backends/branch-and-bound/NodeSet.h"
#include "backends/branch-and-bound-v2/branching-rules/factories/BranchingRuleFactory.h"
#include "backends/branch-and-bound-v2/relaxations/factories/RelaxationBuilderFactory.h"
#include "backends/branch-and-bound-v2/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "backends/branch-and-bound-v2/branching-rules/impls/BranchingRule.h"
#include "backends/branch-and-bound-v2/node-selection-rules/impls/NodeSelectionRule.h"
#include "backends/branch-and-bound-v2/nodes/NodeInfo.h"
#include "backends/branch-and-bound-v2/nodes/Node2.h"
#include "backends/branch-and-bound/Attributes_BranchAndBound.h"

#include <memory>
#include <cassert>

template<class NodeInfoT = NodeInfo>
class BranchAndBoundV2 : public Algorithm {
    using TreeNode = Node2<NodeInfoT>;
    using SetOfActiveNodes = NodeSet<Node2<NodeInfoT>>;

    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;
    std::unique_ptr<RelaxationBuilderFactory> m_relaxation_builder_factory;

    std::unique_ptr<AbstractModel> m_relaxation;
    std::unique_ptr<NodeUpdator2<NodeInfoT>> m_node_updator;

    std::unique_ptr<BranchingRule<NodeInfoT>> m_branching_rule;
    std::unique_ptr<NodeSelectionRule<NodeInfoT>> m_node_selection_rule;

    std::vector<unsigned int> m_steps = { std::numeric_limits<unsigned int>::max(), 1, 0 };
    unsigned int m_n_created_nodes = 0;

    TreeNode* m_incumbent = nullptr;
protected:
    void hook_before_optimize() override;
    void hook_optimize() override;
    void initialize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    void create_relaxations();
    Node2<NodeInfoT>* create_root_node();
    void explore(TreeNode* t_node, SetOfActiveNodes& t_active_nodes, unsigned int t_step);
    void solve(TreeNode* t_node);
    void analyze(TreeNode* t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag);
    Node2<NodeInfoT>* select_node_for_branching(SetOfActiveNodes& t_active_nodes);
    std::vector<TreeNode*> create_child_nodes(const TreeNode* t_node);
    void backtrack(SetOfActiveNodes& t_actives_nodes, SetOfActiveNodes& t_sub_active_nodes);
    void set_as_incumbent(TreeNode* t_node);
    [[nodiscard]] bool gap_is_closed() const;
    void prune_nodes_by_bound(SetOfActiveNodes& t_active_nodes);
    void update_lower_bound(const SetOfActiveNodes& t_active_nodes);

    void log_node(LogLevel t_msg_level, const TreeNode &t_node);
public:
    explicit BranchAndBoundV2(const AbstractModel& t_model,
                              const OptimizerFactory& t_node_optimizer,
                              const RelaxationBuilderFactory& t_relaxation_builder_factory,
                              const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                              const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory);
};

template<class NodeInfoT>
BranchAndBoundV2<NodeInfoT>::BranchAndBoundV2(const AbstractModel &t_model,
                                          const OptimizerFactory& t_node_optimizer,
                                          const RelaxationBuilderFactory& t_relaxation_builder_factory,
                                          const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                                          const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory)
    : Algorithm(t_model),
      m_relaxation_optimizer_factory(t_node_optimizer.clone()),
      m_relaxation_builder_factory(t_relaxation_builder_factory.clone()),
      m_branching_rule(t_branching_rule_factory(t_model)),
      m_node_selection_rule(t_node_selection_rule_factory()) {

    create_relaxations();

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::create_relaxations() {

    const auto &original_model = parent();

    m_relaxation.reset((*m_relaxation_builder_factory)(original_model));
    m_relaxation->use(*m_relaxation_optimizer_factory);

    m_node_updator.reset(NodeInfoT::create_updator(*m_relaxation));

}

template<class NodeInfoT>
Node2<NodeInfoT>* BranchAndBoundV2<NodeInfoT>::create_root_node() {

    auto* root_node = Node2<NodeInfoT>::create_root_node();
    assert(root_node->id() == 0);
    ++m_n_created_nodes;
    return root_node;

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    // Reset incumbent solution
    delete m_incumbent;
    m_incumbent = nullptr;

    m_n_created_nodes = 0;
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::hook_optimize() {

    auto* root_node = create_root_node();

    SetOfActiveNodes active_nodes;

    explore(root_node, active_nodes, 0);

    if (gap_is_closed()) {
        set_status(Optimal);
        set_reason(Proved);
    }

    std::cout << best_obj() << std::endl;

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::explore(TreeNode *t_node,
                                      SetOfActiveNodes & t_active_nodes,
                                      unsigned int t_step) {

    bool reoptimize_flag = false;
    bool explore_children_flag = false;

    do {

        solve(t_node);

        analyze(t_node, &explore_children_flag, &reoptimize_flag);

    } while (reoptimize_flag);

    if (is_terminated() || gap_is_closed()) { return; }

    if (!explore_children_flag) { return; }

    t_active_nodes.emplace(t_node);

    const unsigned int max_levels = m_steps.at(t_step);

    for (unsigned int level = 0 ; level < max_levels ; ++level) {

        prune_nodes_by_bound(t_active_nodes);

        if (t_step == 0) {
            update_lower_bound(t_active_nodes);
        }

        if (t_active_nodes.empty()) { break; }

        auto selected_node = select_node_for_branching(t_active_nodes);

        idol_Log(Trace, BranchAndBoundV2<>, "Node " << selected_node->id() << " was selected for branching.")

        auto children = create_child_nodes(selected_node);

        const unsigned int n_children = children.size();

        std::vector<SetOfActiveNodes> active_nodes(n_children);

        for (unsigned int q = 0 ; q < n_children ; ++q) {
            explore(children[q], active_nodes[q], t_step + 1);
            backtrack(t_active_nodes, active_nodes[q]);
        }

    }

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::solve(TreeNode* t_node) {

    m_node_updator->apply_local_updates(t_node->info());

    m_relaxation->set(Param::Algorithm::BestBoundStop, std::min(best_obj(), get(Param::Algorithm::BestBoundStop)));
    m_relaxation->set(Param::Algorithm::TimeLimit, parent().remaining_time());

    m_relaxation->optimize();

    t_node->save(*m_relaxation);

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::analyze(BranchAndBoundV2::TreeNode *t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag) {

    //if (t_node->id() % get(Param::BranchAndBound::LogFrequency) == 0) {
    if (true) {
        log_node(Info, *t_node);
    }

    const auto& status = t_node->status();
    const auto& reason = t_node->reason();

    if (status == Unbounded) {
        set_status(Unbounded);
        set_reason(Proved);
        set_best_obj(-Inf);
        terminate();
        idol_Log(Trace, BranchAndBoundV2<>, "Terminate. The problem is unbounded.");
        return;
    }

    if (status == Infeasible) {
        idol_Log(Trace, BranchAndBoundV2<>, "Pruning node " << t_node->id() << " (by feasibility).");
        delete t_node;
        return;
    }

    if (status == Feasible && reason == UserObjLimit) {
        idol_Log(Trace, BranchAndBoundV2<>, "Pruning node " << t_node->id() << " (by bound).");
        delete t_node;
        return;
    }

    if (parent().remaining_time() == 0) {
        set_reason(TimeLimit);
        terminate();
        return;
    }

    if (status != Optimal) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        idol_Log(Trace, BranchAndBoundV2<>, "Terminate. Node " << t_node->id() << " could not be solved to optimality.");
        delete t_node;
        return;
    }

    const double objective_value = t_node->objective_value();

    if (m_branching_rule->is_valid(*t_node)) {

        if (objective_value < best_obj()) {
            set_as_incumbent(t_node);
            log_node(Info, *t_node);
            idol_Log(Trace, BranchAndBoundV2<>, "New incumbent with objective value " << objective_value << " found at node " << t_node->id() << ".");
        } else {
            idol_Log(Trace, BranchAndBoundV2<>, "Valid solution with objective value " << objective_value << " found at node " << t_node->id() << '.');
            delete t_node;
        }

        return;

    }

    *t_explore_children_flag = true;

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::log_node(LogLevel t_msg_level, const BranchAndBoundV2::TreeNode &t_node) {

    const double objective_value = t_node.objective_value();
    const unsigned int id = t_node.id();
    char sign = ' ';

    if (equals(objective_value, best_obj(), ToleranceForAbsoluteGapMIP)) {
        sign = '-';
    } else if (equals(objective_value, best_bound(), ToleranceForAbsoluteGapMIP)) {
        sign = '+';
    }

    idol_Log(t_msg_level,
             BranchAndBoundV2<>,
             "<Node=" << (id == -1 ? "H" : std::to_string(id)) << sign << "> "
              << "<Time=" << parent().time().count() << "> "
              << "<Levl=" << t_node.level() << "> "
              << "<Stat=" << (SolutionStatus) t_node.status() << "> "
              << "<Reas=" << (SolutionReason) t_node.reason() << "> "
              << "<ObjV=" << t_node.objective_value() << "> "
              << "<Lb=" << best_bound() << "> "
              << "<Ub=" << best_obj() << "> "
              << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
              << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::set_as_incumbent(BranchAndBoundV2::TreeNode *t_node) {
    delete m_incumbent;
    m_incumbent = t_node;
    set_best_obj(t_node->objective_value());
    set_status(Feasible);
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::update_lower_bound(const BranchAndBoundV2::SetOfActiveNodes &t_active_nodes) {
    if (t_active_nodes.empty()) {
        set_best_bound(best_obj());
        return;
    }
    auto& lowest_node = *t_active_nodes.by_objective_value().begin();
    const double lower_bound = lowest_node.objective_value();
    if (lower_bound > best_bound()) {
        std::cout << lower_bound << std::endl;
        set_best_bound(lower_bound);
        log_node(Info, lowest_node);
    }

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::prune_nodes_by_bound(BranchAndBoundV2::SetOfActiveNodes& t_active_nodes) {

    const double upper_bound = best_obj();

    auto it = t_active_nodes.by_objective_value().begin();
    auto end = t_active_nodes.by_objective_value().end();

    while (it != end) {

        if (const auto& node = *it ; node.objective_value() >= upper_bound) {
            idol_Log(
                    Trace,
                    BranchAndBoundV2<>,
                    "Node " << node.id() << " was pruned by bound "
                            << "(BestObj: " << upper_bound << ", Obj: " << node.objective_value() << ").");
            it = t_active_nodes.erase(it);
            end = t_active_nodes.by_objective_value().end();
            delete &node;
        } else {
            break;
        }

    }

}

template<class NodeInfoT>
Node2<NodeInfoT>*
BranchAndBoundV2<NodeInfoT>::select_node_for_branching(BranchAndBoundV2::SetOfActiveNodes &t_active_nodes) {
    auto iterator = m_node_selection_rule->operator()(t_active_nodes);
    auto* result = iterator.operator->();
    t_active_nodes.erase(iterator);
    return result;
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::write(const std::string &t_name) {
    throw Exception("Not implemented write");
}

template<class NodeInfoT>
std::vector<Node2<NodeInfoT>*> BranchAndBoundV2<NodeInfoT>::create_child_nodes(const BranchAndBoundV2::TreeNode *t_node) {
    auto children_info = m_branching_rule->create_child_nodes(*t_node);

    std::vector<Node2<NodeInfoT>*> result;
    result.reserve(children_info.size());

    for (auto* ptr_to_info : children_info) {
        result.emplace_back(new Node2<NodeInfoT>(ptr_to_info, m_n_created_nodes++, t_node->level() + 1));
    }

    delete t_node;

    return result;
}

template<class NodeInfoT>
bool BranchAndBoundV2<NodeInfoT>::gap_is_closed() const {
    return is_terminated()
        || parent().remaining_time() == 0
        || get(Attr::Solution::RelGap) <= get(Param::Algorithm::MIPGap)
        || get(Attr::Solution::AbsGap) <= get(Param::Algorithm::MIPGap); // TODO these should be different
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::backtrack(BranchAndBoundV2::SetOfActiveNodes &t_actives_nodes,
                                            SetOfActiveNodes &t_sub_active_nodes) {

    t_actives_nodes.merge(std::move(t_sub_active_nodes));

}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::update() {
    throw Exception("Not implemented update");
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::remove(const Ctr &t_ctr) {
    throw Exception("Not implemented remove");
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::remove(const Var &t_var) {
    throw Exception("Not implemented remove");
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::add(const Ctr &t_ctr) {
    throw Exception("Not implemented add");
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::add(const Var &t_var) {
    throw Exception("Not implemented add");
}

template<class NodeInfoT>
void BranchAndBoundV2<NodeInfoT>::initialize() {

}

#endif //IDOL_BRANCHANDBOUNDV2_H
