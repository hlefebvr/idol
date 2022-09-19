//
// Created by henri on 13/09/22.
//
#include "algorithms/branch-and-bound/BranchAndBound.test.h"
#include "solvers.h"

// used for building default branch-and-bound
#include "algorithms/branch-and-bound/ExternalSolverStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
//

#include <iomanip>

BranchAndBound::BranchAndBound(Model &t_model, std::vector<Var> t_branching_candidates) {
    if constexpr (std::tuple_size_v<available_solvers>) {
        set_solution_strategy<ExternalSolverStrategy<std::tuple_element_t<0, available_solvers>>>(t_model);
        set_node_strategy<NodeByBoundStrategy>();
        set_branching_strategy<MostInfeasible>(std::move(t_branching_candidates));
    } else {
        throw std::runtime_error("No available solver.");
    }
}

void BranchAndBound::solve() {

    initialize();
    create_root_node();

    while(!is_terminated()) {

        solve_queued_nodes();

        prune_active_nodes_by_bound();

        update_best_lower_bound();

        if (gap_is_closed()) {
            terminate_for_gap_is_closed();
        }

        if (no_active_nodes()) {
            terminate_for_no_active_nodes();
        }

        branch();

    }

}

template<class T> void free(T& t_container) {
    for (auto* t_ptr : t_container) {
        delete t_ptr;
    }
    t_container = T();
}

double BranchAndBound::relative_gap() const {
    if (is_pos_inf(m_best_upper_bound) || is_neg_inf(m_best_lower_bound)) {
        return Inf;
    }
    return std::abs(m_best_lower_bound - m_best_upper_bound) / (1e-10 + std::abs(m_best_upper_bound));
}

double BranchAndBound::absolute_gap() const {
    if (is_pos_inf(m_best_upper_bound) || is_neg_inf(m_best_lower_bound)) {
        return Inf;
    }
    return std::abs(m_best_lower_bound - m_best_upper_bound);
}

BranchAndBound::~BranchAndBound() {
    free(m_active_nodes);
    free(m_nodes_to_be_processed);
    free(m_solution_pool);
    delete m_current_node;
}

void BranchAndBound::initialize() {
    m_is_terminated = false;
    m_n_created_nodes = 0;
    m_best_lower_bound = -Inf;
    m_best_upper_bound = +Inf;
    m_best_upper_bound_node = nullptr;
    m_current_node = nullptr;
    free(m_active_nodes);
    free(m_nodes_to_be_processed);
    free(m_solution_pool);

    if (!m_solution_strategy) {
        throw std::runtime_error("No solution strategy was given.");
    }

    if (!m_branching_strategy) {
        throw std::runtime_error("No branching strategy was given.");
    }

    if (!m_node_strategy) {
        throw std::runtime_error("No node strategy was given.");
    }

    m_solution_strategy->build();
}

void BranchAndBound::create_root_node() {
    auto* root_node = m_node_strategy->create_root_node();
    if (root_node->id() != 0) {
        throw std::runtime_error("Root node should have id 0.");
    }
    m_nodes_to_be_processed.add(root_node);
    ++m_n_created_nodes;
}

void BranchAndBound::solve_queued_nodes() {

    while (m_nodes_to_be_processed && !is_terminated()) {

        update_current_node();
        prepare_node_solution();
        solve_current_node();

        log_node(Debug, *m_current_node);

        analyze_current_node();

        if (m_current_node) {
            //apply_heuristics_on_current_node();
            add_current_node_to_active_nodes();
        }

    }
}

void BranchAndBound::analyze_current_node() {

    if (current_node_is_root_node()) {

        if (current_node_is_infeasible()) {

            terminate_for_infeasibility();
            return;

        } else if (current_node_is_unbounded()) {

            terminate_for_unboundedness();
            return;

        }

    }

    if (current_node_is_infeasible()) {

        EASY_LOG(Trace, "branch-and-bound", "Pruning node " << m_current_node->id() << " (infeasible).");
        prune_current_node();
        return;

    }

    if (current_node_was_not_solved_to_optimality()) {

        terminate_for_node_could_not_be_solved_to_optimality();
        return;

    }

    if (current_node_has_a_valid_solution()) {

        add_current_node_to_solution_pool();

        EASY_LOG(Trace, "branch-and-bound", "Valid solution found at node " << m_current_node->id() << '.');

        if (current_node_is_below_upper_bound()) {
            set_current_node_as_incumbent();
            log_node(Info, *m_current_node);
            EASY_LOG(Trace, "branch-and-bound", "Better incumbent found at node " << m_current_node->id() << ".");
        }

        reset_current_node();
        return;

    }

    if (current_node_is_above_upper_bound()) {

        EASY_LOG(Trace, "branch-and-bound", "Pruning node " << m_current_node->id() << " (by bound).");
        prune_current_node();
        return;

    }

}

void BranchAndBound::update_current_node() {
    m_current_node = m_nodes_to_be_processed.top();
    m_nodes_to_be_processed.pop();
    EASY_LOG(Trace, "branch-and-bound", "Current node is now node " << m_current_node->id() << '.');
}

void BranchAndBound::prepare_node_solution() {
    m_node_strategy->prepare_node_solution(*m_current_node, *m_solution_strategy);
}

void BranchAndBound::solve_current_node() {
    m_solution_strategy->solve();
    m_current_node->save_solution(*m_solution_strategy);
}

bool BranchAndBound::current_node_is_root_node() const {
    return m_current_node->id() == 0;
}

bool BranchAndBound::current_node_is_infeasible() const {
    return m_current_node->status() == Infeasible;
}

bool BranchAndBound::current_node_is_unbounded() const {
    return m_current_node->status() == Unbounded;
}

bool BranchAndBound::current_node_was_not_solved_to_optimality() const {
    return m_current_node->status() != Optimal;
}

bool BranchAndBound::current_node_has_a_valid_solution() const {
    return m_current_node->status() != Infeasible && m_branching_strategy->is_valid(*m_current_node);
}

AbstractSolutionStrategy& BranchAndBound::set_solution_strategy(AbstractSolutionStrategy*t_node_strategy) {
    m_solution_strategy.reset(t_node_strategy);
    return *m_solution_strategy;
}

AbstractBranchingStrategy& BranchAndBound::set_branching_strategy(AbstractBranchingStrategy *t_branching_strategy) {
    m_branching_strategy.reset(t_branching_strategy);
    return *m_branching_strategy;
}

AbstractNodeStrategy& BranchAndBound::set_node_strategy(AbstractNodeStrategy *t_node_strategy) {
    m_node_strategy.reset(t_node_strategy);
    return *m_node_strategy;
}

bool BranchAndBound::current_node_is_below_upper_bound() {
    return m_current_node->objective_value() < upper_bound();
}

void BranchAndBound::set_current_node_as_incumbent() {
    m_best_upper_bound = m_current_node->objective_value();
    m_best_upper_bound_node = m_current_node;
}

void BranchAndBound::add_current_node_to_solution_pool() {
    m_solution_pool.emplace_back(m_current_node);
}

void BranchAndBound::reset_current_node() {
    m_current_node = nullptr;
}

bool BranchAndBound::current_node_is_above_upper_bound() {
    const double objective_value = m_current_node->objective_value();
    return is_pos_inf(objective_value) || objective_value > upper_bound();
}

void BranchAndBound::apply_heuristics_on_current_node() {
    if (is_terminated()) { return; }
}

void BranchAndBound::prune_current_node() {
    delete m_current_node;
    m_current_node = nullptr;
}

void BranchAndBound::add_current_node_to_active_nodes() {
    m_active_nodes.emplace_back(m_current_node);
    std::push_heap(m_active_nodes.begin(), m_active_nodes.end(), std::less<AbstractNode*>());
    m_current_node = nullptr;
}

void BranchAndBound::prune_active_nodes_by_bound() {
    for (auto it = m_active_nodes.begin(), end = m_active_nodes.end() ; it != end ; ++it) {
        const auto* ptr_to_node = *it;
        if (ptr_to_node->objective_value() >= upper_bound()) {
            EASY_LOG(Trace, "branch-and-bound", "[NODE_PRUNED] value = node " << ptr_to_node->id() << ".");
            delete ptr_to_node;
            it = m_active_nodes.erase(it);
        }
    }
    std::make_heap(m_active_nodes.begin(), m_active_nodes.end(), std::less<AbstractNode*>());
}

void BranchAndBound::update_best_lower_bound() {

    if (m_active_nodes.empty()) {
        m_best_lower_bound = m_best_upper_bound;
        return;
    }

    auto* min_node = m_active_nodes.front();
    if (double lb = min_node->objective_value() ; lb > lower_bound()) {
        m_best_lower_bound = lb;
        log_node(Info, *min_node);
    }
}

bool BranchAndBound::no_active_nodes() {
    return m_active_nodes.empty();
}

void BranchAndBound::branch() {

    if (is_terminated()) { return; }

    auto* selected_node = m_active_nodes.front();
    std::pop_heap(m_active_nodes.begin(), m_active_nodes.end(), std::less<AbstractNode*>());
    m_active_nodes.pop_back();
    EASY_LOG(Trace, "branch-and-bound", "Node " << selected_node->id() << " has been selected for branching.");

    auto child_nodes = m_branching_strategy->create_child_nodes(m_n_created_nodes, *selected_node);

    for (auto* node : child_nodes) {
        if (node->id() != m_n_created_nodes) {
            throw std::runtime_error("Created nodes should have strictly increasing ids.");
        }
        EASY_LOG(Trace, "branch-and-bound", "Node " << node->id() << " has been created from " << selected_node->id() << '.');
        m_nodes_to_be_processed.add(node);
        ++m_n_created_nodes;
    }

    delete selected_node;
}

void BranchAndBound::terminate_for_no_active_nodes() {
    EASY_LOG(Trace, "branch-and-bound", "Terminate. No active node.");
    terminate();
}

void BranchAndBound::terminate_for_gap_is_closed() {
    EASY_LOG(Trace, "branch-and-bound", "Terminate. Gap is closed.");
    terminate();
}

void BranchAndBound::terminate_for_infeasibility() {
    EASY_LOG(Trace, "branch-and-bound", "Terminate. Infeasibility detected.");
    terminate();
}

void BranchAndBound::terminate_for_unboundedness() {
    m_best_upper_bound = -Inf;
    EASY_LOG(Trace, "branch-and-bound", "Terminate. Unboundedness detected.");
    terminate();
}

void BranchAndBound::terminate_for_node_could_not_be_solved_to_optimality() {
    EASY_LOG(Trace, "branch-and-bound", "Terminate. Current node could node be solved to optimality (node " << m_current_node->id() << ").");
    terminate();
}

void BranchAndBound::terminate() {
    m_is_terminated = true;
}

void BranchAndBound::log_node(LogLevel t_msg_level, const AbstractNode& t_node) const {

    const double objective_value = t_node.objective_value();
    char sign = ' ';

    if (equals(objective_value, upper_bound(), ToleranceForAbsoluteGapMIP)) {
        sign = '-';
    } else if (equals(objective_value, lower_bound(), ToleranceForAbsoluteGapMIP)) {
        sign = '+';
    }

    EASY_LOG(t_msg_level,
             "branch-and-bound",
             std::setw(4)
             << t_node.id() << sign
             << std::setw(15)
             << t_node.status()
             << std::setw(10)
             << t_node.objective_value()
             << std::setw(10)
             << lower_bound()
             << std::setw(10)
             << upper_bound()
             << std::setw(10)
             << (relative_gap() * 100.) << '%'
     );
}

SolutionStatus BranchAndBound::status() const {
    if (is_neg_inf(m_best_upper_bound)) {
        return Unbounded;
    }
    if (m_best_upper_bound_node) {
        if (gap_is_closed()) {
            return Optimal;
        }
        return Feasible;
    }
    return Infeasible;
}

double BranchAndBound::objective_value() const {
    return m_best_upper_bound;
}

Solution::Primal BranchAndBound::primal_solution() const {
    if (!m_best_upper_bound_node) {
        throw std::runtime_error("Not available.");
    }
    return m_best_upper_bound_node->primal_solution();
}
