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
#include "optimizers/branch-and-bound/callbacks/AbstractBranchAndBoundCallbackI.h"
#include "optimizers/Logger.h"
#include "optimizers/OptimizerFactory.h"
#include "modeling/models/Model.h"

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

    std::unique_ptr<AbstractBranchAndBoundCallbackI<NodeInfoT>> m_callback;

    unsigned int m_log_frequency = 10;
    std::vector<unsigned int> m_steps = { std::numeric_limits<unsigned int>::max(), 1, 0 };
    unsigned int m_n_created_nodes = 0;
    unsigned int m_n_solved_nodes = 0;
    double m_root_node_best_bound = -Inf;
    double m_root_node_best_obj = +Inf;

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

    SideEffectRegistry call_callbacks(CallbackEvent t_event, TreeNode* t_node);

    void log_node(LogLevel t_msg_level, const TreeNode &t_node);

    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;
    void set_solution_index(unsigned int t_index) override;

    void update_obj_sense() override;
    void update_obj() override;
    void update_rhs() override;
    void update_obj_constant() override;
    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;
    void update_ctr_type(const Ctr &t_ctr) override;
    void update_ctr_rhs(const Ctr &t_ctr) override;
    void update_var_type(const Var &t_var) override;
    void update_var_lb(const Var &t_var) override;
    void update_var_ub(const Var &t_var) override;
    void update_var_obj(const Var &t_var) override;

public:
    explicit BranchAndBound(const Model& t_model,
                              const OptimizerFactory& t_node_optimizer,
                              const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                              const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory,
                              AbstractBranchAndBoundCallbackI<NodeInfoT>* t_callback);

    ~BranchAndBound() override;

    [[nodiscard]] std::string name() const override { return "branch-and-bound"; }

    virtual void set_log_frequency(unsigned int t_log_frequency) { m_log_frequency = t_log_frequency; }

    [[nodiscard]] unsigned int log_frequency() const { return m_log_frequency; }

    virtual void set_subtree_depth(unsigned int t_depth) { m_steps.at(1) = t_depth; }

    [[nodiscard]] unsigned int subtree_depth() const { return m_steps.at(1); }

    virtual void add_callback(BranchAndBoundCallback<NodeInfoT>* t_cb);

    virtual void add_cutting_plane_generator(const CuttingPlaneGenerator& t_cutting_plane_generator);

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_lower_bound(double t_lower_bound);

    [[nodiscard]] unsigned int n_created_nodes() const { return m_n_created_nodes; }

    [[nodiscard]] unsigned int n_solved_nodes() const { return m_n_solved_nodes; }

    [[nodiscard]] const Model& relaxation() const { return *m_relaxation; }

    [[nodiscard]] double root_node_best_bound() const { return m_root_node_best_bound; }

    [[nodiscard]] double root_node_best_obj() const { return m_root_node_best_obj; }
};

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::add_cutting_plane_generator(const CuttingPlaneGenerator &t_cutting_plane_generator) {
    m_callback->add_cutting_plane_generator(t_cutting_plane_generator);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds.");
    }
}

template<class NodeInfoT>
unsigned int Optimizers::BranchAndBound<NodeInfoT>::get_solution_index() const {
    return 0;
}

template<class NodeInfoT>
unsigned int Optimizers::BranchAndBound<NodeInfoT>::get_n_solutions() const {
    return !!m_incumbent;
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_obj() {
    m_relaxation->set_obj_expr(parent().get_obj_expr());
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_rhs() {
    m_relaxation->set_rhs_expr(parent().get_rhs_expr());
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_obj_constant() {
    m_relaxation->set_obj_const(parent().get_obj_expr().constant());
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {
    m_relaxation->set_mat_coeff(t_ctr, t_var, parent().get_mat_coeff(t_ctr, t_var));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_ctr_type(const Ctr &t_ctr) {
    m_relaxation->set_ctr_type(t_ctr, parent().get_ctr_type(t_ctr));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_ctr_rhs(const Ctr &t_ctr) {
    m_relaxation->set_ctr_rhs(t_ctr, parent().get_ctr_row(t_ctr).rhs());
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_var_type(const Var &t_var) {
    m_relaxation->set_var_type(t_var, parent().get_var_type(t_var));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_var_lb(const Var &t_var) {
    m_relaxation->set_var_lb(t_var, parent().get_var_lb(t_var));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_var_ub(const Var &t_var) {
    m_relaxation->set_var_ub(t_var, parent().get_var_ub(t_var));
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_var_obj(const Var &t_var) {
    m_relaxation->set_var_obj(t_var, parent().get_var_column(t_var).obj());
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::update_obj_sense() {
    throw Exception("Not implemented");
}

template<class NodeInfoT>
double Optimizers::BranchAndBound<NodeInfoT>::get_ctr_farkas(const Ctr &t_ctr) const {
    if (m_n_solved_nodes > 1) {
        throw Exception("Farkas certificate not available.");
    }
    return m_relaxation->get_ctr_farkas(t_ctr);
}

template<class NodeInfoT>
double Optimizers::BranchAndBound<NodeInfoT>::get_ctr_dual(const Ctr &t_ctr) const {
    if (m_n_solved_nodes > 1) {
        throw Exception("Dual value not available.");
    }
    return m_relaxation->get_ctr_dual(t_ctr);
}

template<class NodeInfoT>
double Optimizers::BranchAndBound<NodeInfoT>::get_var_ray(const Var &t_var) const {
    if (m_n_solved_nodes > 1) {
        throw Exception("Ray not available.");
    }
    return m_relaxation->get_var_ray(t_var);
}

template<class NodeInfoT>
double Optimizers::BranchAndBound<NodeInfoT>::get_var_primal(const Var &t_var) const {

    if (!m_incumbent){
        throw Exception("Primal value not available.");
    }

    return m_incumbent->info().primal_solution().get(t_var);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::submit_lower_bound(double t_lower_bound) {
    if (t_lower_bound > get_best_obj()) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        return;
    }
    if (t_lower_bound > get_best_bound()) {
        set_best_bound(t_lower_bound);
        idol_Log(Trace, "New best lower bound of " << t_lower_bound << " was submitted.");
    }
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::submit_heuristic_solution(NodeInfoT* t_info) {

    auto* t_node = new Node<NodeInfoT>(t_info, -1, 0);

    if (t_node->objective_value() < get_best_obj()) {

        if (m_branching_rule->is_valid(*t_node)) {
            set_as_incumbent(t_node);
            log_node(Info, *t_node);
            idol_Log(Trace, "New incumbent with objective value " << t_node->objective_value() << " was submitted by heuristic.");
        } else {
            idol_Log(Trace, "Ignoring submitted heuristic solution, solution is not valid.");
            delete t_node;
        }

    } else {
        idol_Log(Trace, "Ignoring submitted heuristic solution, objective value is " << t_node->objective_value() << " while best obj is " << get_best_obj() << '.');
        delete t_node;
    }


}

template<class NodeInfoT>
SideEffectRegistry
Optimizers::BranchAndBound<NodeInfoT>::call_callbacks(CallbackEvent t_event, Optimizers::BranchAndBound<NodeInfoT>::TreeNode *t_node) {
    if (!m_callback) {
        return {};
    }
    return m_callback->operator()(this, t_event, t_node, m_relaxation.get());
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::add_callback(BranchAndBoundCallback<NodeInfoT> *t_cb) {
    m_callback->add_callback(t_cb);
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
Optimizers::BranchAndBound<NodeInfoT>::BranchAndBound(const Model &t_model,
                                                      const OptimizerFactory& t_node_optimizer,
                                                      const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                                                      const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory,
                                                      AbstractBranchAndBoundCallbackI<NodeInfoT>* t_callback)
    : Algorithm(t_model),
      m_relaxation_optimizer_factory(t_node_optimizer.clone()),
      m_branching_rule(t_branching_rule_factory(t_model)),
      m_node_selection_rule(t_node_selection_rule_factory()),
      m_callback(t_callback) {

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
    m_n_solved_nodes = 0;

    m_root_node_best_bound = -Inf;
    m_root_node_best_obj = Inf;

    m_callback->initialize(*m_relaxation);
}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::hook_optimize() {

    auto* root_node = create_root_node();

    SetOfActiveNodes active_nodes;

    explore(root_node, active_nodes, 0);

    if (active_nodes.empty()) {
        set_best_bound(get_best_obj());
    }

    m_node_updator->clear_local_updates();

    std::cout << "Solved " << m_n_solved_nodes << " nodes" << std::endl;

    if (!m_incumbent) {

        if (is_pos_inf(get_best_obj())) {
            set_status(Infeasible);
            return;
        } else if (is_neg_inf(get_best_obj())) {
            set_status(Unbounded);
            return;
        }

    } else {
        set_status(Feasible);
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

    if (is_terminated() || gap_is_closed()) { return; }

    bool reoptimize_flag;
    bool explore_children_flag;

    do {

        solve(t_node);

        analyze(t_node, &explore_children_flag, &reoptimize_flag);

    } while (reoptimize_flag);

    ++m_n_solved_nodes;

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

        idol_Log(Trace, "Node " << selected_node->id() << " was selected for branching.")

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

    idol_Log(Trace, "Preparing to solve node " << t_node->id() << ".");

    m_node_updator->apply_local_updates(t_node->info());

    m_relaxation->optimizer().set_best_bound_stop(std::min(get_best_obj(), best_bound_stop()));
    m_relaxation->optimizer().set_time_limit(get_remaining_time());

    idol_Log(Debug, "Beginning to solve node " << t_node->id() << ".");

    m_relaxation->optimize();

    idol_Log(Debug, "Node " << t_node->id() << " has been solved.");

    t_node->save(parent(), *m_relaxation);

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::analyze(BranchAndBound::TreeNode *t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag) {

    *t_explore_children_flag = false;
    *t_reoptimize_flag = false;

    if (get_best_bound() > get_best_obj()) {
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
        idol_Log(Trace, "Terminate. The problem is unbounded.");
        delete t_node;
        return;
    }

    if (status == Infeasible) {
        idol_Log(Trace, "Pruning node " << t_node->id() << " (by feasibility).");
        delete t_node;
        return;
    }

    if (status == Feasible && reason == ObjLimit) {
        idol_Log(Trace, "Node " << t_node->id() << " was pruned by bound " << "(BestObj: " << get_best_obj() << ", Obj: " << t_node->objective_value() << ").");
        delete t_node;
        return;
    }

    if (get_remaining_time() == 0) {
        set_reason(TimeLimit);
        terminate();
        delete t_node;
        return;
    }

    if (status != Optimal) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        idol_Log(Trace, "Terminate. Node " << t_node->id() << " could not be solved to optimality (status = " << status << ").");
        delete t_node;
        return;
    }

    if (t_node->level() == 0) {
        m_root_node_best_bound = t_node->objective_value();
    }

    if (t_node->objective_value() < get_best_obj()) {

        if (m_branching_rule->is_valid(*t_node)) {

            auto side_effects = call_callbacks(IncumbentSolution, t_node);

            if (side_effects.n_added_user_cuts > 0 || side_effects.n_added_lazy_cuts > 0) {
                *t_reoptimize_flag = true;
                idol_Log(Trace, "The relaxation was modified by callback at node " << t_node->id() << ". Re-optimization is needed.");
                return;
            }

            set_as_incumbent(t_node);
            log_node(Info, *t_node);
            idol_Log(Trace, "New incumbent with objective value " << t_node->objective_value() << " found at node " << t_node->id() << ".");

            return;

        }

    } else {

        idol_Log(Trace, "Node " << t_node->id() << " was pruned by bound " << "(BestObj: " << get_best_obj() << ", Obj: " << t_node->objective_value() << ").");
        delete t_node;
        return;

    }

    auto side_effects = call_callbacks(InvalidSolution, t_node);

    if (t_node->level() == 0) {
        m_root_node_best_obj = get_best_obj();
    }

    if (side_effects.n_added_lazy_cuts > 0 || side_effects.n_added_user_cuts > 0) {
        *t_reoptimize_flag = true;
    }

    *t_explore_children_flag = true;

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::log_node(LogLevel t_msg_level, const BranchAndBound::TreeNode &t_node) {

    const double objective_value = t_node.objective_value();
    const unsigned int id = t_node.id();
    char sign = ' ';

    if (equals(objective_value, get_best_obj(), ToleranceForAbsoluteGapMIP)) {
        sign = '-';
    } else if (equals(objective_value, get_best_bound(), ToleranceForAbsoluteGapMIP)) {
        sign = '+';
    }

    idol_Log(t_msg_level,
             "<Node="     << std::setw(4) << (id == -1 ? "H" : std::to_string(id)) << sign << "> "
              << "<TimT=" << std::setw(7) << time().count() << "> "
              << "<TimI=" << std::setw(7) << m_relaxation.get()->optimizer().time().count() << "> "
              << "<Levl=" << std::setw(3) << t_node.level() << "> "
              << "<Stat=" << t_node.status() << "> "
              << "<Reas=" << t_node.reason() << "> "
              << "<ObjVal=" << std::setw(9) << t_node.objective_value() << "> "
              << "<BestBnd="   << std::setw(9) << get_best_bound() << "> "
              << "<BestObj="   << std::setw(9) << get_best_obj() << "> "
              << "<RelGap=" << std::setw(5) << get_relative_gap() * 100 << "> "
              << "<AbsGap=" << std::setw(5) << get_absolute_gap() << "> "
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
    if (lower_bound > get_best_bound()) {
        set_best_bound(lower_bound);
        log_node(Info, lowest_node);
    }

}

template<class NodeInfoT>
void Optimizers::BranchAndBound<NodeInfoT>::prune_nodes_by_bound(BranchAndBound::SetOfActiveNodes& t_active_nodes) {

    const double upper_bound = get_best_obj();

    auto it = t_active_nodes.by_objective_value().begin();
    auto end = t_active_nodes.by_objective_value().end();

    while (it != end) {

        if (const auto& node = *it ; node.objective_value() >= upper_bound) {
            idol_Log(Trace, "Node " << node.id() << " was pruned by bound " << "(BestObj: " << get_best_obj() << ", Obj: " << node.objective_value() << ").");
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
           || get_remaining_time() == 0
           || get_relative_gap() <= relative_gap_tolerance()
        || get_absolute_gap() <= absolute_gap_tolerance();
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
