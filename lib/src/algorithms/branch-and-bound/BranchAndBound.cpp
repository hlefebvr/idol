//
// Created by henri on 13/09/22.
//
#include "../../../include/algorithms/branch-and-bound/BranchAndBound.h"
#include "../../../include/algorithms/branch-and-bound/ActiveNodesManager.h"
#include "../../../include/algorithms/callbacks/Callback.h"
#include "../../../include/algorithms/callbacks/BranchAndBoundCallback.h"

#include <iomanip>

void BranchAndBound::execute() {

    initialize();
    create_root_node();

    while(!is_terminated()) {

        solve_queued_nodes();

        prune_active_nodes_by_bound();

        update_best_lower_bound();

        if (m_best_lower_bound > m_best_upper_bound) {
            terminate_for_error_lb_greater_than_ub();
        }

        ++m_iteration;

        if (!is_terminated() && gap_is_closed()) {
            terminate_for_gap_is_closed();
        }

        if (!is_terminated() && no_active_nodes()) {
            terminate_for_no_active_nodes();
        }

        if (!is_terminated() && iteration_limit_is_reached()) {
            terminate_for_iteration_limit_is_reached();
        }

        if (!is_terminated() && time_limit_is_reached()) {
            terminate_for_time_limit_is_reached();
        }

        branch();

    }

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
    return std::abs(m_best_upper_bound - m_best_lower_bound);
}

void BranchAndBound::initialize() {
    m_is_terminated = false;
    m_n_created_nodes = 0;
    m_iteration = 0;
    m_best_lower_bound = -Inf;
    m_best_upper_bound = std::min(+Inf, m_solution_strategy->get(Param::Algorithm::BestObjStop));

    if (!m_solution_strategy) {
        throw Exception("No solution strategy was given.");
    }

    if (!m_nodes) {
        throw Exception("No node storage strategy was given");
    }

    m_nodes->initialize();
}

void BranchAndBound::create_root_node() {
    m_nodes->create_root_node();
    ++m_n_created_nodes;
}

void BranchAndBound::solve_queued_nodes() {

    while (
            m_nodes->has_node_to_be_processed()
            && !is_terminated()
        ) {

        m_nodes->set_current_node_to_next_node_to_be_processed();

        idol_Log(Trace,
                 "branch-and-bound",
                 "Current node is now node " << m_nodes->current_node().id() << '.'
                 );

        prepare_node_solution();
        solve_current_node();

        log_node(Debug, m_nodes->current_node());

        analyze_current_node();

        if (m_nodes->has_current_node()) {
            apply_heuristics_on_current_node();
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

    } else if(m_iteration % 10 == 0) {
        log_node(Info, current_node());
    }

    if (current_node_is_infeasible()) {

        idol_Log(Trace, "branch-and-bound", "Pruning node " << m_nodes->current_node().id() << " (infeasible).");
        prune_current_node();
        return;

    }

    if (current_node_was_not_solved_to_optimality()) {

        terminate_for_node_could_not_be_solved_to_optimality();
        return;

    }

    if (current_node_has_a_valid_solution()) {

        add_current_node_to_solution_pool();

        idol_Log(Trace, "branch-and-bound", "Valid solution found at node " << current_node().id() << '.');

        if (current_node_is_below_upper_bound()) {
            set_current_node_as_incumbent();
            m_best_upper_bound = current_node().objective_value();
            log_node(Info, m_nodes->current_node());
            idol_Log(Trace, "branch-and-bound", "Better incumbent found at node " << current_node().id() << ".");
        }

        reset_current_node();
        return;

    }

    if (current_node_is_above_upper_bound()) {

        idol_Log(Trace, "branch-and-bound", "Pruning node " << current_node().id() << " (by bound).");
        prune_current_node();
        return;

    }

}

void BranchAndBound::prepare_node_solution() {
    m_nodes->apply_current_node_to(*m_solution_strategy);
}

void BranchAndBound::solve_current_node() {
    if (m_solution_strategy->sense() == Minimize) {
        m_solution_strategy->set(Param::Algorithm::BestBoundStop, std::min(m_best_upper_bound, get(Param::Algorithm::BestBoundStop)));
    } else {
        m_solution_strategy->set(Param::Algorithm::BestObjStop, std::min(m_best_upper_bound, get(Param::Algorithm::BestObjStop)));
    }

    const double remaining_time = std::max(0., get(Param::Algorithm::TimeLimit) - time().count());
    m_solution_strategy->set(Param::Algorithm::TimeLimit, remaining_time);

    m_solution_strategy->solve();
    m_nodes->save_current_node_solution(*m_solution_strategy);
}

bool BranchAndBound::current_node_is_root_node() const {
    return current_node().id() == 0;
}

bool BranchAndBound::current_node_is_infeasible() const {
    return current_node().status() == Infeasible;
}

bool BranchAndBound::current_node_is_unbounded() const {
    return current_node().status() == Unbounded;
}

bool BranchAndBound::current_node_was_not_solved_to_optimality() const {
    auto status = current_node().status();
    if (status == Unknown) {
        return current_node().reason() != UserObjLimit && current_node().reason() != CutOff;
    }
    return status != Optimal;
}

bool BranchAndBound::current_node_has_a_valid_solution() const {
    return m_nodes->current_node_has_a_valid_solution();
}

bool BranchAndBound::current_node_is_below_upper_bound() {
    return current_node().objective_value() < m_best_upper_bound;
}

void BranchAndBound::set_current_node_as_incumbent() {
    m_nodes->set_current_node_as_incumbent();
}

void BranchAndBound::add_current_node_to_solution_pool() {
    m_nodes->add_current_node_to_solution_pool();
}

void BranchAndBound::reset_current_node() {
    m_nodes->reset_current_node();
}

bool BranchAndBound::current_node_is_above_upper_bound() {
    const double objective_value = current_node().objective_value();
    return is_pos_inf(objective_value) || objective_value > std::min(get(Param::Algorithm::CutOff), m_best_upper_bound);
}

void BranchAndBound::apply_heuristics_on_current_node() {

    for (const auto& ptr_to_cb : m_callbacks) {
        if (is_terminated()) { break; }
        BranchAndBound::Callback::AdvancedContext ctx(*this, RelaxationSolved);
        ptr_to_cb->execute(ctx);
    }

}

void BranchAndBound::prune_current_node() {
    m_nodes->prune_current_node();
}

void BranchAndBound::add_current_node_to_active_nodes() {
    m_nodes->add_current_node_to_active_nodes();
}

void BranchAndBound::prune_active_nodes_by_bound() {
    m_nodes->active_nodes().prune_by_bound(std::min(get(Param::Algorithm::CutOff), m_best_upper_bound));
}

void BranchAndBound::update_best_lower_bound() {
    if (m_nodes->active_nodes().empty()) {
        m_best_lower_bound = m_best_upper_bound;
        return;
    }

    const auto& lowest_node = m_nodes->active_nodes().lowest_node();
    if (double lb = lowest_node.objective_value() ; lb > m_best_lower_bound) {
        m_best_lower_bound = lb;
        log_node(Info, lowest_node);
    }
}

bool BranchAndBound::no_active_nodes() {
    return m_nodes->active_nodes().empty();
}

void BranchAndBound::branch() {

    if (is_terminated()) { return; }

    m_nodes->active_nodes().select_node_for_branching();

    m_n_created_nodes += m_nodes->create_child_nodes();

    m_nodes->active_nodes().remove_node_selected_for_branching();
}

void BranchAndBound::terminate_for_no_active_nodes() {
    idol_Log(Trace, "branch-and-bound", "Terminate. No active node.");
    set_status(m_nodes->has_incumbent() ? Optimal : Infeasible);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_gap_is_closed() {
    idol_Log(Trace, "branch-and-bound", "Terminate. Gap is closed.");
    set_status(Optimal);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_infeasibility() {
    idol_Log(Trace, "branch-and-bound", "Terminate. Infeasibility detected.");
    set_status(Infeasible);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_unboundedness() {
    m_best_upper_bound = -Inf;
    idol_Log(Trace, "branch-and-bound", "Terminate. Unboundedness detected.");
    set_status(Unbounded);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_node_could_not_be_solved_to_optimality() {
    idol_Log(Trace, "branch-and-bound", "Terminate. Current node could node be solved to optimality (node " << current_node().id() << ").");
    set_status(Fail);
    set_reason(NotSpecified);
    terminate();
}

void BranchAndBound::terminate_for_iteration_limit_is_reached() {
    idol_Log(Trace, "branch-and-bound", "Terminate. The maximum number of iterations has been reached.")
    set_status(m_nodes->has_incumbent() ? Feasible : Infeasible);
    set_reason(IterationCount);
    terminate();
}

void BranchAndBound::terminate_for_error_lb_greater_than_ub() {
    idol_Log(Trace, "branch-and-bound", "Terminate. Best LB > Best UB.");
    set_status(Fail);
    set_reason(NotSpecified);
    terminate();
}

void BranchAndBound::terminate() {
    m_is_terminated = true;
}

void BranchAndBound::log_node(LogLevel t_msg_level, const Node& t_node) const {

    const double objective_value = t_node.objective_value();
    const unsigned int id = t_node.id();
    char sign = ' ';

    if (equals(objective_value, m_best_upper_bound, ToleranceForAbsoluteGapMIP)) {
        sign = '-';
    } else if (equals(objective_value, m_best_lower_bound, ToleranceForAbsoluteGapMIP)) {
        sign = '+';
    }

    idol_Log(t_msg_level,
             "branch-and-bound",
             std::setw(4)
             << (id == -1 ? "H" : std::to_string(id)) << sign
             << std::setw(5)
             << m_nodes->active_nodes().size()
             << std::setw(10)
             << t_node.status()
             << std::setw(10)
             << t_node.reason()
             << std::setw(15)
             << t_node.objective_value()
             << std::setw(15)
             << m_best_lower_bound
             << std::setw(15)
             << m_best_upper_bound
             << std::setw(10)
             << (relative_gap() * 100.) << '%'
             << std::setw(10)
             << time().count()
     );
}

double BranchAndBound::objective_value() const {
    return m_best_upper_bound;
}

Solution::Primal BranchAndBound::primal_solution() const {
    if (!m_nodes->has_incumbent()) {
        throw Exception("Not available.");
    }
    return m_nodes->incumbent().primal_solution();
}

const Node &BranchAndBound::current_node() const {
    return m_nodes->current_node();
}

double BranchAndBound::lower_bound() const {
    return m_best_lower_bound;
}

double BranchAndBound::upper_bound() const {
    return m_best_upper_bound;
}

bool BranchAndBound::iteration_limit_is_reached() const {
    return m_iteration >= get(Param::Algorithm::MaxIterations);
}

bool BranchAndBound::submit_solution(Solution::Primal &&t_solution) {
    if (m_nodes->submit_solution(std::move(t_solution), m_best_upper_bound)) {
        idol_Log(Debug, "branch-and-bound", "New incumbent solution found by submission.");
        m_best_upper_bound = m_nodes->incumbent().objective_value();
        log_node(Info, m_nodes->incumbent());
        return true;
    }
    return false;
}

bool BranchAndBound::time_limit_is_reached() const {
    return get(Param::Algorithm::TimeLimit) <= time().count();
}

void BranchAndBound::terminate_for_time_limit_is_reached() {
    if (is_terminated()) { return; }
    idol_Log(Trace, "branch-and-bound", "Terminate. Time limit is reached.");
    set_status(m_nodes->has_incumbent() ? Feasible : Infeasible);
    set_reason(TimeLimit);
    terminate();
}

bool BranchAndBound::set_parameter_int(const Parameter<int> &t_param, int t_value) {
    if (t_param.is_in_section(Param::Sections::BranchAndBound)) {
        m_params_int.set(t_param, t_value);
        return true;
    }
    return Algorithm::set_parameter_int(t_param, t_value);;
}

std::optional<int> BranchAndBound::get_parameter_int(const Parameter<int> &t_param) const {
    if (t_param.is_in_section(Param::Sections::BranchAndBound)) {
        return m_params_int.get(t_param);
    }
    return Algorithm::get_parameter_int(t_param);
}

AttributeManager &BranchAndBound::attribute_delegate(const Attribute &t_attribute) {
    return *m_solution_strategy;
}

AttributeManager &BranchAndBound::attribute_delegate(const Attribute &t_attribute, const Var &t_object) {
    return *m_solution_strategy;
}

AttributeManager &BranchAndBound::attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) {
    return *m_solution_strategy;
}

bool BranchAndBound::set_parameter_double(const Parameter<double> &t_param, double t_value) {
    if (t_param == Param::Algorithm::BestObjStop) {
        m_best_upper_bound = t_value;
    }
    return Algorithm::set_parameter_double(t_param, t_value);
}
