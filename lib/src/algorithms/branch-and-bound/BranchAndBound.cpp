//
// Created by henri on 13/09/22.
//
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "solvers/Solver.h"

void BranchAndBound::solve() {

    initialize();
    create_root_node();

    int i = 0;

    while(!gap_is_closed()) {

        solve_queued_nodes();

        prune_active_nodes_by_bound();

        update_best_lower_bound();

        if (no_active_nodes()) {
            terminate();
            break;
        }

        branch();

        std::cout << "STOPPING MANUALLY" << std::endl;

        if (i > 3) {
            break;
        }
        ++i;

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
    delete m_best_upper_bound_node;
    delete m_current_node;
}

void BranchAndBound::initialize() {
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

    m_solution_strategy->initialize();
}

void BranchAndBound::create_root_node() {
    auto* root_node = m_node_strategy->create_root_node(m_n_created_nodes);
    m_nodes_to_be_processed.add(root_node);
    ++m_n_created_nodes;
}

void BranchAndBound::solve_queued_nodes() {
    while (m_nodes_to_be_processed) {

        update_current_node();
        apply_local_changes();
        solve_current_node();

        if (current_node_was_not_solved_to_optimality()) {
            terminate();
            break;
        }

        if (current_node_has_a_valid_solution()) {
            if (current_node_is_below_upper_bound()) {
                set_current_node_as_incumbent();
            }
            add_current_node_to_solution_pool();
            reset_current_node();
            reset_local_changes();
            continue;
        }

        if (current_node_is_above_upper_bound()) {
            prune_current_node();
            reset_local_changes();
            continue;
        }

        //apply_heuristics_on_current_node();

        add_current_node_to_active_nodes();

        reset_local_changes();

    }
}

void BranchAndBound::update_current_node() {
    m_current_node = m_nodes_to_be_processed.top();
    m_nodes_to_be_processed.pop();
}

void BranchAndBound::apply_local_changes() {
    m_current_node->apply_local_changes(*m_solution_strategy);
}

void BranchAndBound::solve_current_node() {
    m_solution_strategy->solve();
    m_current_node->save_solution(*m_solution_strategy);
}

bool BranchAndBound::current_node_was_not_solved_to_optimality() const {
    return m_current_node->status() != Optimal;
}

bool BranchAndBound::current_node_has_a_valid_solution() const {
    return m_branching_strategy->is_valid(*m_current_node);
}

void BranchAndBound::set_solution_strategy(AbstractSolutionStrategy*t_node_strategy) {
    m_solution_strategy.reset(t_node_strategy);
}

void BranchAndBound::set_branching_strategy(AbstractBranchingStrategy *t_branching_strategy) {
    m_branching_strategy.reset(t_branching_strategy);
}

void BranchAndBound::set_node_strategy(AbstractNodeStrategy *t_node_strategy) {
    m_node_strategy.reset(t_node_strategy);
}

bool BranchAndBound::current_node_is_below_upper_bound() {
    return m_current_node->objective_value() < upper_bound();
}

void BranchAndBound::set_current_node_as_incumbent() {
    m_best_upper_bound = m_current_node->objective_value();
}

void BranchAndBound::add_current_node_to_solution_pool() {
    m_solution_pool.emplace_back(m_current_node);
}

void BranchAndBound::reset_current_node() {
    m_current_node = nullptr;
}

bool BranchAndBound::current_node_is_above_upper_bound() {
    return m_current_node->objective_value() > upper_bound();
}

void BranchAndBound::apply_heuristics_on_current_node() {

}

void BranchAndBound::prune_current_node() {
    delete m_current_node;
    m_current_node = nullptr;
}

void BranchAndBound::add_current_node_to_active_nodes() {
    m_active_nodes.emplace_back(m_current_node);
    std::push_heap(m_active_nodes.begin(), m_active_nodes.end());
    m_current_node = nullptr;
}

void BranchAndBound::reset_local_changes() {
    m_solution_strategy->reset_local_changes();
}

void BranchAndBound::prune_active_nodes_by_bound() {
    for (auto it = m_active_nodes.begin(), end = m_active_nodes.end() ; it != end ; ++it) {
        const auto* ptr_to_node = *it;
        if (ptr_to_node->objective_value() >= upper_bound()) {
            delete ptr_to_node;
            it = m_active_nodes.erase(it);
        }
    }
    std::make_heap(m_active_nodes.begin(), m_active_nodes.end());
}

void BranchAndBound::update_best_lower_bound() {
    auto* min_node = m_active_nodes.front();
    if (double lb = min_node->objective_value() ; lb > lower_bound()) {
        m_best_lower_bound = lb;
    }
}

bool BranchAndBound::no_active_nodes() {
    return m_active_nodes.empty();
}

void BranchAndBound::branch() {
    auto* selected_node = m_active_nodes.front();// m_branching_strategy->select_node_for_branching(m_active_nodes);
    std::pop_heap(m_active_nodes.begin(), m_active_nodes.end());
    m_active_nodes.pop_back();
    auto child_nodes = m_branching_strategy->create_child_nodes(m_active_nodes.size(), *selected_node);
    for (auto* node : child_nodes) {
        m_nodes_to_be_processed.add(node);
    }
}

void BranchAndBound::terminate() {
    throw std::runtime_error("Called terminate.");
}

