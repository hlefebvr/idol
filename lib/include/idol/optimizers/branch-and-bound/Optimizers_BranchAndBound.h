//
// Created by henri on 13/03/23.
//

#ifndef IDOL_OPTIMIZERS_BRANCHANDBOUND_H
#define IDOL_OPTIMIZERS_BRANCHANDBOUND_H

#include "idol/optimizers/branch-and-bound/nodes/NodeSet.h"
#include "idol/optimizers/Algorithm.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/NodeSelectionRuleFactory.h"
#include "idol/optimizers/branch-and-bound/branching-rules/impls/BranchingRule.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/impls/NodeSelectionRule.h"
#include "idol/optimizers/branch-and-bound/nodes/Node.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeUpdator.h"
#include "idol/optimizers/branch-and-bound/callbacks/AbstractBranchAndBoundCallbackI.h"
#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/models/Model.h"
#include "idol/optimizers/branch-and-bound/logs/LoggerFactory.h"

#include <memory>
#include <cassert>

namespace idol::Optimizers {
    template<class NodeInfoT> class BranchAndBound;
}

template<class NodeInfoT>
class idol::Optimizers::BranchAndBound : public Algorithm {
    using TreeNode = Node<NodeInfoT>;
    using SetOfActiveNodes = NodeSet<Node<NodeInfoT>>;

    std::unique_ptr<OptimizerFactory> m_relaxation_optimizer_factory;

    std::unique_ptr<Model> m_relaxation;
    std::unique_ptr<NodeUpdator<NodeInfoT>> m_node_updator;

    std::unique_ptr<BranchingRule<NodeInfoT>> m_branching_rule;
    std::unique_ptr<NodeSelectionRule<NodeInfoT>> m_node_selection_rule;
    std::unique_ptr<typename LoggerFactory<NodeInfoT>::Strategy> m_logger;

    std::unique_ptr<AbstractBranchAndBoundCallbackI<NodeInfoT>> m_callback;

    bool m_perform_scaling = false;
    std::vector<unsigned int> m_steps = { std::numeric_limits<unsigned int>::max(), 0, 0 };
    unsigned int m_n_created_nodes = 0;
    unsigned int m_n_solved_nodes = 0;
    double m_root_node_best_bound = -Inf;
    double m_root_node_best_obj = +Inf;

    std::optional<TreeNode> m_incumbent;
protected:
    void build() override;
    void hook_before_optimize() override;
    void hook_optimize() override;
    void hook_after_optimize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    void create_relaxations();
    Node<NodeInfoT> create_root_node();
    void explore(TreeNode& t_node, SetOfActiveNodes& t_active_nodes, unsigned int t_step);
    void analyze(const TreeNode& t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag);
    Node<NodeInfoT> select_node_for_branching(SetOfActiveNodes& t_active_nodes);
    std::vector<TreeNode> create_child_nodes(const TreeNode& t_node);
    void backtrack(SetOfActiveNodes& t_actives_nodes, SetOfActiveNodes& t_sub_active_nodes);
    void set_as_incumbent(const TreeNode& t_node);
    [[nodiscard]] bool gap_is_closed() const;
    void prune_nodes_by_bound(SetOfActiveNodes& t_active_nodes);
    void update_lower_bound(const SetOfActiveNodes& t_active_nodes);

    void log_node_after_solve(const Node<NodeInfoT>& t_node);
    void log_after_termination();

    SideEffectRegistry call_callbacks(CallbackEvent t_event, const TreeNode& t_node);

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
                              AbstractBranchAndBoundCallbackI<NodeInfoT>* t_callback,
                              bool t_scaling,
                              const LoggerFactory<NodeInfoT>& t_logger_factory);

    [[nodiscard]] std::string name() const override { return "Branch-and-Bound"; }

    void solve(TreeNode& t_node) const;

    virtual void set_subtree_depth(unsigned int t_depth) { m_steps.at(1) = t_depth; }

    [[nodiscard]] unsigned int subtree_depth() const { return m_steps.at(1); }

    virtual void add_callback(BranchAndBoundCallback<NodeInfoT>* t_cb);

    void submit_heuristic_solution(NodeInfoT* t_info);

    void submit_lower_bound(double t_lower_bound);

    [[nodiscard]] unsigned int n_created_nodes() const { return m_n_created_nodes; }

    [[nodiscard]] unsigned int n_solved_nodes() const { return m_n_solved_nodes; }

    [[nodiscard]] const Model& relaxation() const { return *m_relaxation; }

    [[nodiscard]] Model& relaxation() { return *m_relaxation; }

    [[nodiscard]] double root_node_best_bound() const { return m_root_node_best_bound; }

    [[nodiscard]] double root_node_best_obj() const { return m_root_node_best_obj; }

    BranchingRule<NodeInfoT>& branching_rule() { return *m_branching_rule; }

    const BranchingRule<NodeInfoT>& branching_rule() const { return *m_branching_rule; }

    [[nodiscard]] bool has_incumbent() const { return m_incumbent.has_value(); }

    const TreeNode& incumbent() const { return m_incumbent.value(); }

    [[nodiscard]] double get_var_primal(const Var &t_var) const override;

    [[nodiscard]] double get_var_ray(const Var &t_var) const override;

    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;

    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;

    [[nodiscard]] unsigned int get_n_solutions() const override;

    [[nodiscard]] unsigned int get_solution_index() const override;

    void set_solution_index(unsigned int t_index) override;
};

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::log_after_termination() {

    if (!get_param_logs()) {
        return;
    }

    if (m_callback) {
        m_callback->log_after_termination();
    }

    m_logger->log_after_termination();

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::log_node_after_solve(const idol::Node<NodeInfoT> &t_node) {

    if (!get_param_logs()) {
        return;
    }

    m_logger->log_node_after_solve(t_node);

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::set_solution_index(unsigned int t_index) {
    if (t_index != 0) {
        throw Exception("Solution index out of bounds.");
    }
}

template<class NodeInfoT>
unsigned int idol::Optimizers::BranchAndBound<NodeInfoT>::get_solution_index() const {
    return 0;
}

template<class NodeInfoT>
unsigned int idol::Optimizers::BranchAndBound<NodeInfoT>::get_n_solutions() const {
    return !!m_incumbent;
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_obj() {
    m_relaxation->set_obj_expr(parent().get_obj_expr());
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_rhs() {
    m_relaxation->set_rhs_expr(parent().get_rhs_expr());
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_obj_constant() {
    m_relaxation->set_obj_const(parent().get_obj_expr().constant());
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_mat_coeff(const Ctr &t_ctr, const Var &t_var) {
    m_relaxation->set_mat_coeff(t_ctr, t_var, parent().get_mat_coeff(t_ctr, t_var));
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_ctr_type(const Ctr &t_ctr) {
    m_relaxation->set_ctr_type(t_ctr, parent().get_ctr_type(t_ctr));
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_ctr_rhs(const Ctr &t_ctr) {
    m_relaxation->set_ctr_rhs(t_ctr, parent().get_ctr_row(t_ctr).rhs());
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_var_type(const Var &t_var) {
    m_relaxation->set_var_type(t_var, parent().get_var_type(t_var));
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_var_lb(const Var &t_var) {
    m_relaxation->set_var_lb(t_var, parent().get_var_lb(t_var));
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_var_ub(const Var &t_var) {
    m_relaxation->set_var_ub(t_var, parent().get_var_ub(t_var));
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_var_obj(const Var &t_var) {
    m_relaxation->set_var_obj(t_var, parent().get_var_column(t_var).obj());
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_obj_sense() {
    throw Exception("Not implemented");
}

template<class NodeInfoT>
double idol::Optimizers::BranchAndBound<NodeInfoT>::get_ctr_farkas(const Ctr &t_ctr) const {
    if (m_n_solved_nodes > 1) {
        throw Exception("Accessing Farkas certificate for MIP is not available after the root node.");
    }
    return m_relaxation->get_ctr_farkas(t_ctr);
}

template<class NodeInfoT>
double idol::Optimizers::BranchAndBound<NodeInfoT>::get_ctr_dual(const Ctr &t_ctr) const {
    if (m_n_solved_nodes > 1) {
        throw Exception("Accessing dual values for MIP is not available after the root node.");
    }
    return m_relaxation->get_ctr_dual(t_ctr);
}

template<class NodeInfoT>
double idol::Optimizers::BranchAndBound<NodeInfoT>::get_var_ray(const Var &t_var) const {
    if (m_n_solved_nodes > 1) {
        throw Exception("Ray not available.");
    }
    return m_relaxation->get_var_ray(t_var);
}

template<class NodeInfoT>
double idol::Optimizers::BranchAndBound<NodeInfoT>::get_var_primal(const Var &t_var) const {

    if (!m_incumbent.has_value()){
        throw Exception("Trying to access primal values while no incumbent was found.");
    }

    return m_incumbent->info().primal_solution().get(t_var);
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::submit_lower_bound(double t_lower_bound) {
    if (t_lower_bound > get_best_obj()) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        return;
    }
    if (t_lower_bound > get_best_bound()) {
        set_best_bound(t_lower_bound);
        // New best LB
    }
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::submit_heuristic_solution(NodeInfoT* t_info) {

    auto t_node = Node<NodeInfoT>::create_detached_node(t_info);

    if (t_node.info().objective_value() < get_best_obj()) {

        //if (m_branching_rule->is_valid(t_node)) {
            set_as_incumbent(t_node);
            // New incumbent by submission
        //} else {
        //    idol_Log(Trace, "Ignoring submitted heuristic solution, solution is not valid.");
        //}

    } else {
        // idol_Log(Trace, "Ignoring submitted heuristic solution, objective value is " << t_node.info().objective_value() << " while best obj is " << get_best_obj() << '.');
    }

}

template<class NodeInfoT>
idol::SideEffectRegistry
idol::Optimizers::BranchAndBound<NodeInfoT>::call_callbacks(CallbackEvent t_event, const Optimizers::BranchAndBound<NodeInfoT>::TreeNode &t_node) {

    if (!m_callback) {
        return {};
    }

    return m_callback->operator()(this, t_event, t_node, m_relaxation.get());
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::add_callback(BranchAndBoundCallback<NodeInfoT> *t_cb) {
    m_callback->add_callback(t_cb);
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::build() {

}

template<class NodeInfoT>
idol::Optimizers::BranchAndBound<NodeInfoT>::BranchAndBound(const Model &t_model,
                                                      const OptimizerFactory& t_node_optimizer,
                                                      const BranchingRuleFactory<NodeInfoT>& t_branching_rule_factory,
                                                      const NodeSelectionRuleFactory<NodeInfoT>& t_node_selection_rule_factory,
                                                      AbstractBranchAndBoundCallbackI<NodeInfoT>* t_callback,
                                                      bool t_scaling,
                                                      const LoggerFactory<NodeInfoT>& t_logger_factory)
    : Algorithm(t_model),
      m_relaxation_optimizer_factory(t_node_optimizer.clone()),
      m_branching_rule(t_branching_rule_factory(*this)),
      m_node_selection_rule(t_node_selection_rule_factory(*this)),
      m_callback(t_callback),
      m_perform_scaling(t_scaling),
      m_logger(t_logger_factory.operator()(*this)) {

    create_relaxations();

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::create_relaxations() {

    const auto &original_model = parent();

    m_relaxation.reset(original_model.clone());
    if (m_perform_scaling) {
        m_relaxation->scale_to_integers(Tolerance::Digits);
    }
    m_relaxation->use(*m_relaxation_optimizer_factory);

    m_node_updator.reset(dynamic_cast<NodeUpdator<NodeInfoT>*>(NodeInfoT::create_updator(*m_relaxation)));

}

template<class NodeInfoT>
idol::Node<NodeInfoT> idol::Optimizers::BranchAndBound<NodeInfoT>::create_root_node() {

    auto root_node = Node<NodeInfoT>::create_root_node();
    assert(root_node.id() == 0);
    ++m_n_created_nodes;
    return root_node;

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::hook_before_optimize() {
    Algorithm::hook_before_optimize();

    // Reset solution
    set_best_bound(std::max(-Inf, get_param_best_obj_stop()));
    set_best_obj(std::min(+Inf, get_param_best_bound_stop()));
    m_incumbent.reset();

    m_n_created_nodes = 0;
    m_n_solved_nodes = 0;

    m_root_node_best_bound = -Inf;
    m_root_node_best_obj = Inf;

    m_branching_rule->initialize();
    m_node_updator->initialize();
    m_callback->initialize(this);

    m_logger->initialize();
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::hook_optimize() {

    auto root_node = create_root_node();

    SetOfActiveNodes active_nodes;

    explore(root_node, active_nodes, 0);

    if (active_nodes.empty()) {
        set_best_bound(get_best_obj());
    }

    m_node_updator->clear();

    if (get_status() == Fail) {
        return;
    }

    if (!m_incumbent.has_value()) {

        if (is_pos_inf(get_best_obj())) {
            set_status(Infeasible);
            return;
        }

        if (is_neg_inf(get_best_obj())) {
            set_status(Unbounded);
            return;
        }

    }

    set_status(Feasible);

    if (gap_is_closed()) {
        set_status(Optimal);
        set_reason(Proved);
        return;
    }

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::hook_after_optimize() {

    Optimizer::hook_after_optimize();

    log_after_termination();

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::explore(TreeNode &t_node,
                                                    SetOfActiveNodes & t_active_nodes,
                                                    unsigned int t_step) {

    if (is_terminated() || gap_is_closed()) { return; }

    bool reoptimize_flag;
    bool explore_children_flag;

    do {

        solve(t_node);

        analyze(t_node, &explore_children_flag, &reoptimize_flag);

        log_node_after_solve(t_node);

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
        
        if (is_terminated() || gap_is_closed()) { break; }

        auto selected_node = select_node_for_branching(t_active_nodes);

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
void idol::Optimizers::BranchAndBound<NodeInfoT>::solve(TreeNode& t_node) const {

    m_node_updator->prepare(t_node);

    m_relaxation->optimizer().set_param_best_bound_stop(std::min(get_best_obj(), get_param_best_bound_stop()));
    m_relaxation->optimizer().set_param_time_limit(get_remaining_time());

    m_relaxation->optimize();

    t_node.info().save(parent(), *m_relaxation);

    m_branching_rule->on_node_solved(t_node);

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::analyze(const BranchAndBound::TreeNode &t_node, bool* t_explore_children_flag, bool* t_reoptimize_flag) {

    *t_explore_children_flag = false;
    *t_reoptimize_flag = false;

    if (get_best_bound() > get_best_obj()) {
        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        return;
    }

    const auto& status = t_node.info().status();
    const auto& reason = t_node.info().reason();

    if (get_remaining_time() == 0 || reason == TimeLimit) {
        set_reason(TimeLimit);
        terminate();
        return;
    }

    if (status == Unbounded) {
        set_reason(Proved);
        set_best_obj(-Inf);
        terminate();
        return;
    }

    if (status == Infeasible || status == InfOrUnbnd) {

        if (t_node.level() == 0) {
            set_status(status);
        }

        call_callbacks(PrunedSolution, t_node);
        return;
    }

    if (status == Feasible && reason == ObjLimit) {
        call_callbacks(PrunedSolution, t_node);
        return;
    }

    if (status == Fail || status == Loaded) {

        /*
        if (m_n_postponed_nodes < m_max_postponed_nodes) {
            *t_explore_children_flag = true;
            std::cout << "Postponing Node " << t_node.id() << " since returned status is " << status << "." << std::endl;
            idol_Log(Trace, "Postponing Node " << t_node.id() << " since returned status is " << status << ".");
            ++m_n_postponed_nodes;
            return;
        }
         */

        set_status(Fail);
        set_reason(NotSpecified);
        terminate();
        call_callbacks(PrunedSolution, t_node);
        return;
    }

    if (t_node.level() == 0) {
        m_root_node_best_bound = t_node.info().objective_value();
    }

    if (t_node.info().objective_value() < get_best_obj()) {

        if (m_branching_rule->is_valid(t_node)) {

            auto side_effects = call_callbacks(IncumbentSolution, t_node);

            if (side_effects.n_added_user_cuts > 0 || side_effects.n_added_lazy_cuts > 0) {
                *t_reoptimize_flag = true;
                return;
            }

            set_as_incumbent(t_node);
            return;

        }

    } else {

        call_callbacks(PrunedSolution, t_node);
        return;

    }

    auto side_effects = call_callbacks(InvalidSolution, t_node);

    if (t_node.level() == 0) {
        m_root_node_best_obj = get_best_obj();
    }

    if (side_effects.n_added_lazy_cuts > 0 || side_effects.n_added_user_cuts > 0) {
        *t_reoptimize_flag = true;
        return;
    }

    *t_explore_children_flag = true;

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::set_as_incumbent(const BranchAndBound::TreeNode &t_node) {
    m_incumbent = t_node;
    set_best_obj(t_node.info().objective_value());
    set_status(Feasible);
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update_lower_bound(const BranchAndBound::SetOfActiveNodes &t_active_nodes) {

    if (t_active_nodes.empty()) { return; }

    auto& lowest_node = *t_active_nodes.by_objective_value().begin();
    const double lower_bound = lowest_node.info().objective_value();
    if (lower_bound > get_best_bound()) {
        set_best_bound(lower_bound);
    }

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::prune_nodes_by_bound(BranchAndBound::SetOfActiveNodes& t_active_nodes) {

    const double upper_bound = get_best_obj();

    auto it = t_active_nodes.by_objective_value().begin();
    auto end = t_active_nodes.by_objective_value().end();

    while (it != end) {

        if (const auto& node = *it ; node.info().objective_value() >= upper_bound) {
            it = t_active_nodes.erase(it);
            end = t_active_nodes.by_objective_value().end();
        } else {
            break;
        }

    }

}

template<class NodeInfoT>
idol::Node<NodeInfoT>
idol::Optimizers::BranchAndBound<NodeInfoT>::select_node_for_branching(BranchAndBound::SetOfActiveNodes &t_active_nodes) {
    auto iterator = m_node_selection_rule->operator()(t_active_nodes);
    auto result = *iterator;
    t_active_nodes.erase(iterator);
    return result;
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::write(const std::string &t_name) {
    m_relaxation->write(t_name);
}

template<class NodeInfoT>
std::vector<idol::Node<NodeInfoT>> idol::Optimizers::BranchAndBound<NodeInfoT>::create_child_nodes(const BranchAndBound::TreeNode &t_node) {

    auto children_info = m_branching_rule->create_child_nodes(t_node);

    std::vector<Node<NodeInfoT>> result;
    result.reserve(children_info.size());

    for (auto* ptr_to_info : children_info) {
        result.emplace_back(ptr_to_info, m_n_created_nodes++, t_node);
    }

    m_branching_rule->on_nodes_have_been_created();

    return result;
}

template<class NodeInfoT>
bool idol::Optimizers::BranchAndBound<NodeInfoT>::gap_is_closed() const {
    return get_relative_gap() <= get_tol_mip_relative_gap()
           || get_absolute_gap() <= get_tol_mip_absolute_gap();
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::backtrack(BranchAndBound::SetOfActiveNodes &t_actives_nodes,
                                                      SetOfActiveNodes &t_sub_active_nodes) {

    t_actives_nodes.merge(std::move(t_sub_active_nodes));

}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::update() {
    m_relaxation->update();
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::remove(const Ctr &t_ctr) {
    m_relaxation->remove(t_ctr);
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::remove(const Var &t_var) {
    m_relaxation->remove(t_var);
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::add(const Ctr &t_ctr) {
    m_relaxation->add(t_ctr);
}

template<class NodeInfoT>
void idol::Optimizers::BranchAndBound<NodeInfoT>::add(const Var &t_var) {
    m_relaxation->add(t_var);
}

#endif //IDOL_OPTIMIZERS_BRANCHANDBOUND_H
