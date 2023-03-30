//
// Created by henri on 13/03/23.
//

#ifndef IDOL_OPTIMIZERS_BRANCHANDBOUND_H
#define IDOL_OPTIMIZERS_BRANCHANDBOUND_H

#include "NodeSet.h"
#include "../Algorithm.h"
#include "optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "optimizers/branch-and-bound/branching-rules/impls/BranchingRule.h"
#include "optimizers/branch-and-bound/node-selection-rules/impls/NodeSelectionRule.h"
#include "optimizers/branch-and-bound/nodes/Node.h"
#include "optimizers/branch-and-bound/nodes/NodeUpdator.h"
#include "optimizers/Logger.h"
#include "callbacks/CallbackI.h"

#include <memory>
#include <cassert>

namespace Optimizers {
    template<class NodeInfoT> class BranchAndBound;
}

template<class NodeInfoT>
class Optimizers::BranchAndBound : public Algorithm {
    using TreeNode = Node<NodeInfoT>;
    using SetOfActiveNodes = NodeSet<Node<NodeInfoT>>;

    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;

    std::unique_ptr<Model> m_relaxation;
    std::unique_ptr<NodeUpdator<NodeInfoT>> m_node_updator;

    std::unique_ptr<BranchingRule<NodeInfoT>> m_branching_rule;
    std::unique_ptr<NodeSelectionRule<NodeInfoT>> m_node_selection_rule;

    std::list<CallbackI<NodeInfoT>*> m_callbacks;

    unsigned int m_log_frequency = 10;
    std::vector<unsigned int> m_steps = { std::numeric_limits<unsigned int>::max(), 1, 0 };
    unsigned int m_n_created_nodes = 0;

    TreeNode* m_incumbent = nullptr;
protected:
    void build() override;
    void hook_before_optimize() override;
    void hook_optimize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    void create_relaxations();
    Node<NodeInfoT>* create_root_node();
    void explore(TreeNode* t_node, SetOfActiveNodes& t_active_nodes, unsigned int t_step);
    void solve(TreeNode* t_node);
    void analyze(TreeNode* t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag);
    Node<NodeInfoT>* select_node_for_branching(SetOfActiveNodes& t_active_nodes);
    std::vector<TreeNode*> create_child_nodes(const TreeNode* t_node);
    void backtrack(SetOfActiveNodes& t_actives_nodes, SetOfActiveNodes& t_sub_active_nodes);
    void set_as_incumbent(TreeNode* t_node);
    [[nodiscard]] bool gap_is_closed() const;
    void prune_nodes_by_bound(SetOfActiveNodes& t_active_nodes);
    void update_lower_bound(const SetOfActiveNodes& t_active_nodes);

    void call_callbacks(BranchAndBoundEvent t_event, TreeNode* t_node);

    void log_node(LogLevel t_msg_level, const TreeNode &t_node);

    using Algorithm::get;
    using Algorithm::set;

    [[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override;
    [[nodiscard]] double get(const Req<double, Ctr>& t_attr, const Ctr& t_ctr) const override;
    [[nodiscard]] const Expr<Var, Var>& get(const Req<Expr<Var, Var>, void>& t_attr) const override;
    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override;
    void set(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override;
    void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override;
public:
    explicit BranchAndBound(const Model& t_model,
                              const OptimizerFactory& t_node_optimizer,
                              const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                              const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory);

    ~BranchAndBound() override;

    [[nodiscard]] std::string name() const override { return "branch-and-bound"; }

    virtual void set_log_frequency(unsigned int t_log_frequency) { m_log_frequency = t_log_frequency; }

    [[nodiscard]] unsigned int log_frequency() const { return m_log_frequency; }

    virtual void set_subtree_depth(unsigned int t_depth) { m_steps.at(1) = t_depth; }

    [[nodiscard]] unsigned int subtree_depth() const { return m_steps.at(1); }

    virtual void add_callback(CallbackI<NodeInfoT>* t_cb);

    void submit_heuristic_solution(NodeInfoT* t_info);
};

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::submit_heuristic_solution(NodeInfoT* t_info) {

    auto* t_node = new Node<NodeInfoT>(t_info, -1, 0);

    if (t_node->objective_value() < best_obj()) {

        if (m_branching_rule->is_valid(*t_node)) {
            set_as_incumbent(t_node);
            log_node(Info, *t_node);
            idol_Log2(Trace, "New incumbent with objective value " << t_node->objective_value() << " was submitted by heuristic.");
        } else {
            idol_Log2(Trace, "Ignoring submitted heuristic solution, solution is not valid.");
            delete t_node;
        }

    } else {
        idol_Log2(Trace, "Ignoring submitted heuristic solution, objective value is " << t_node->objective_value() << " while best obj is " << best_obj() << '.');
        delete t_node;
    }


}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::call_callbacks(BranchAndBoundEvent t_event, Optimizers::BranchAndBound<NodeInfoT>::TreeNode *t_node) {

    for (auto* cb : m_callbacks) {

        cb->set_relaxation(m_relaxation.get());
        cb->set_node(t_node);
        cb->set_parent(this);

        cb->operator()(t_event);

        cb->set_node(nullptr);
        cb->set_relaxation(nullptr);
        cb->set_parent(nullptr);

    }

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::add_callback(CallbackI<NodeInfoT> *t_cb) {
    m_callbacks.emplace_back(t_cb);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::build() {
    m_branching_rule->m_log_level = log_level();
    m_branching_rule->m_log_color = log_color();
}

template<class NodeInfoT>
Optimizers::BranchAndBound<NodeInfoT>::~BranchAndBound() {
    delete m_incumbent;
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {
    m_relaxation->set(t_attr, std::move(t_value));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {
    m_relaxation->set(t_attr, t_ctr, std::move(t_value));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {
    m_relaxation->set(t_attr, t_var, t_value);
}

template<class NodeInfoT>
const Expr<Var, Var> &Optimizers::BranchAndBound<NodeInfoT>::get(const Req<Expr<Var, Var>, void> &t_attr) const {
    return m_relaxation->get(t_attr);
}

template<class NodeInfoT>
double Optimizers::BranchAndBound<NodeInfoT>::get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const {
    return m_relaxation->get(t_attr, t_ctr);
}

template<class NodeInfoT>
double Optimizers::BranchAndBound<NodeInfoT>::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Solution::Primal) {
        if (m_incumbent) {
            return m_incumbent->info().primal_solution().get(t_var);
        } else {
            throw Exception("No incumbent found.");
        }
    }

    return Base::get(t_attr, t_var);
}

template<class NodeInfoT>
Optimizers::BranchAndBound<NodeInfoT>::BranchAndBound(const Model &t_model,
                                                      const OptimizerFactory& t_node_optimizer,
                                                      const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                                                      const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory)
    : Algorithm(t_model),
      m_relaxation_optimizer_factory(t_node_optimizer.clone()),
      m_branching_rule(t_branching_rule_factory(t_model)),
      m_node_selection_rule(t_node_selection_rule_factory()) {

    create_relaxations();

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::create_relaxations() {

    const auto &original_model = parent();

    m_relaxation.reset(original_model.clone());
    m_relaxation->use(*m_relaxation_optimizer_factory);

    m_node_updator.reset(NodeInfoT::create_updator(*m_relaxation));

}

template<class NodeInfoT>
Node<NodeInfoT>* Optimizers::BranchAndBound<NodeInfoT>::create_root_node() {

    auto* root_node = Node<NodeInfoT>::create_root_node();
    assert(root_node->id() == 0);
    ++m_n_created_nodes;
    return root_node;

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    // Reset solution
    set_best_bound(std::max(-Inf, best_obj_stop()));
    set_best_obj(std::min(+Inf, best_bound_stop()));
    delete m_incumbent;
    m_incumbent = nullptr;

    m_n_created_nodes = 0;
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::hook_optimize() {

    auto* root_node = create_root_node();

    SetOfActiveNodes active_nodes;

    explore(root_node, active_nodes, 0);

    if (active_nodes.empty()) {
        set_best_bound(best_obj());
    }

    m_node_updator->clear_local_updates();

    if (!m_incumbent) {
        if (is_pos_inf(best_obj())) {
            set_status(Infeasible);
        } else {
            set_status(Unbounded);
        }
        return;
    }

    if (gap_is_closed()) {
        set_status(Optimal);
        set_reason(Proved);
        return;
    }

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::explore(TreeNode *t_node,
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

        idol_Log2(Trace, "Node " << selected_node->id() << " was selected for branching.")

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
void Optimizers::BranchAndBound<NodeInfoT>::solve(TreeNode* t_node) {

    idol_Log2(Trace, "Preparing to solve node " << t_node->id() << ".");

    m_node_updator->apply_local_updates(t_node->info());

    m_relaxation->optimizer().set_best_bound_stop(std::min(best_obj(), best_bound_stop()));
    m_relaxation->optimizer().set_time_limit(remaining_time());

    idol_Log2(Debug, "Beginning to solve node " << t_node->id() << ".");

    m_relaxation->optimize();

    idol_Log2(Debug, "Node " << t_node->id() << " has been solved.");

    t_node->save(parent(), *m_relaxation);

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::analyze(BranchAndBound::TreeNode *t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag) {

    if (best_bound() > best_obj()) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        delete t_node;
        return;
    }

    if (t_node->id() % m_log_frequency == 0) {
        log_node(Info, *t_node);
    }

    const auto& status = t_node->status();
    const auto& reason = t_node->reason();

    if (status == Unbounded) {
        set_reason(Proved);
        set_best_obj(-Inf);
        terminate();
        idol_Log2(Trace, "Terminate. The problem is unbounded.");
        delete t_node;
        return;
    }

    if (status == Infeasible) {
        idol_Log2(Trace, "Pruning node " << t_node->id() << " (by feasibility).");
        delete t_node;
        return;
    }

    if (status == Feasible && reason == UserObjLimit) {
        idol_Log2(Trace, "Node " << t_node->id() << " was pruned by bound " << "(BestObj: " << best_obj() << ", Obj: " << t_node->objective_value() << ").");
        delete t_node;
        return;
    }

    if (remaining_time() == 0) {
        set_reason(TimeLimit);
        terminate();
        delete t_node;
        return;
    }

    if (status != Optimal) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        idol_Log2(Trace, "Terminate. Node " << t_node->id() << " could not be solved to optimality.");
        delete t_node;
        return;
    }

    if (t_node->objective_value() < best_obj()) {

        if (m_branching_rule->is_valid(*t_node)) {

            set_as_incumbent(t_node);
            log_node(Info, *t_node);
            idol_Log2(Trace, "New incumbent with objective value " << t_node->objective_value() << " found at node " << t_node->id() << ".");
            return;

        }

    } else {

        idol_Log2(Trace, "Node " << t_node->id() << " was pruned by bound " << "(BestObj: " << best_obj() << ", Obj: " << t_node->objective_value() << ").");
        delete t_node;
        return;

    }

    call_callbacks(InvalidSolutionFound, t_node);

    *t_explore_children_flag = true;

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::log_node(LogLevel t_msg_level, const BranchAndBound::TreeNode &t_node) {

    const double objective_value = t_node.objective_value();
    const unsigned int id = t_node.id();
    char sign = ' ';

    if (equals(objective_value, best_obj(), ToleranceForAbsoluteGapMIP)) {
        sign = '-';
    } else if (equals(objective_value, best_bound(), ToleranceForAbsoluteGapMIP)) {
        sign = '+';
    }

    idol_Log2(t_msg_level,
             "<Node=" << (id == -1 ? "H" : std::to_string(id)) << sign << "> "
              << "<Time=" << time().count() << "> "
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
void Optimizers::BranchAndBound<NodeInfoT>::set_as_incumbent(BranchAndBound::TreeNode *t_node) {
    delete m_incumbent;
    m_incumbent = t_node;
    set_best_obj(t_node->objective_value());
    set_status(Feasible);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_lower_bound(const BranchAndBound::SetOfActiveNodes &t_active_nodes) {

    if (t_active_nodes.empty()) { return; }

    auto& lowest_node = *t_active_nodes.by_objective_value().begin();
    const double lower_bound = lowest_node.objective_value();
    if (lower_bound > best_bound()) {
        set_best_bound(lower_bound);
        log_node(Info, lowest_node);
    }

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::prune_nodes_by_bound(BranchAndBound::SetOfActiveNodes& t_active_nodes) {

    const double upper_bound = best_obj();

    auto it = t_active_nodes.by_objective_value().begin();
    auto end = t_active_nodes.by_objective_value().end();

    while (it != end) {

        if (const auto& node = *it ; node.objective_value() >= upper_bound) {
            idol_Log2(Trace, "Node " << node.id() << " was pruned by bound " << "(BestObj: " << best_obj() << ", Obj: " << node.objective_value() << ").");
            it = t_active_nodes.erase(it);
            end = t_active_nodes.by_objective_value().end();
            delete &node;
        } else {
            break;
        }

    }

}

template<class NodeInfoT>
Node<NodeInfoT>*
Optimizers::BranchAndBound<NodeInfoT>::select_node_for_branching(BranchAndBound::SetOfActiveNodes &t_active_nodes) {
    auto iterator = m_node_selection_rule->operator()(t_active_nodes);
    auto* result = iterator.operator->();
    t_active_nodes.erase(iterator);
    return result;
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::write(const std::string &t_name) {
    m_relaxation->write(t_name);
}

template<class NodeInfoT>
std::vector<Node<NodeInfoT>*> Optimizers::BranchAndBound<NodeInfoT>::create_child_nodes(const BranchAndBound::TreeNode *t_node) {
    auto children_info = m_branching_rule->create_child_nodes(*t_node);

    std::vector<Node<NodeInfoT>*> result;
    result.reserve(children_info.size());

    for (auto* ptr_to_info : children_info) {
        result.emplace_back(new Node<NodeInfoT>(ptr_to_info, m_n_created_nodes++, t_node->level() + 1));
    }

    delete t_node;

    return result;
}

template<class NodeInfoT>
bool Optimizers::BranchAndBound<NodeInfoT>::gap_is_closed() const {
    return is_terminated()
        || remaining_time() == 0
        || get(Attr::Solution::RelGap) <= relative_gap_tolerance()
        || get(Attr::Solution::AbsGap) <= absolute_gap_tolerance();
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::backtrack(BranchAndBound::SetOfActiveNodes &t_actives_nodes,
                                                      SetOfActiveNodes &t_sub_active_nodes) {

    t_actives_nodes.merge(std::move(t_sub_active_nodes));

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update() {
    m_relaxation->update();
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::remove(const Ctr &t_ctr) {
    m_relaxation->remove(t_ctr);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::remove(const Var &t_var) {
    m_relaxation->remove(t_var);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::add(const Ctr &t_ctr) {
    m_relaxation->add(t_ctr);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::add(const Var &t_var) {
    m_relaxation->add(t_var);
}

#endif //IDOL_OPTIMIZERS_BRANCHANDBOUND_H
