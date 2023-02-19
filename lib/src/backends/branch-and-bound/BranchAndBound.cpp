//
// Created by henri on 01/02/23.
//
#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/branch-and-bound/ActiveNodesManager.h"
#include "backends/branch-and-bound/Node.h"

BranchAndBound::BranchAndBound(const AbstractModel &t_model) : Algorithm(t_model) {

}

void BranchAndBound::initialize() {

}

void BranchAndBound::add(const Var &t_var) {

    const auto& model = parent();
    const auto& column = model.get(Attr::Var::Column, t_var);
    const double lb = model.get(Attr::Var::Lb, t_var);
    const double ub = model.get(Attr::Var::Ub, t_var);
    const int type = model.get(Attr::Var::Type, t_var);

    m_relaxations.get().model().add(t_var, TempVar(lb, ub, type, Column(column)));

}

void BranchAndBound::add(const Ctr &t_ctr) {

    const auto& model = parent();
    const auto& row = model.get(Attr::Ctr::Row, t_ctr);
    const int type = model.get(Attr::Ctr::Type, t_ctr);

    m_relaxations.get().model().add(t_ctr, TempCtr(Row(row), type));
}

void BranchAndBound::remove(const Var &t_var) {
    m_relaxations.get().model().remove(t_var);
}

void BranchAndBound::remove(const Ctr &t_ctr) {
    m_relaxations.get().model().remove(t_ctr);
}

void BranchAndBound::update() {
    m_relaxations.get().model().update();
}

void BranchAndBound::write(const std::string &t_name) {
    m_relaxations.get().model().write(t_name);
}

void BranchAndBound::hook_before_optimize() {

    m_n_created_nodes = 0;
    m_iteration = 0;
    set_best_bound(std::max(-Inf, get(Param::Algorithm::BestObjStop)));
    set_best_obj(std::min(+Inf, get(Param::Algorithm::BestBoundStop)));

    if (!m_nodes_manager) {
        throw Exception("No nodes manager was given.");
    }

    m_nodes_manager->initialize();

}

void BranchAndBound::hook_optimize() {

    initialize();
    create_root_node();

    //call_callback(Event_::Algorithm::Begin);

    while(!is_terminated()) {

        solve_queued_nodes();

        prune_active_nodes_by_bound();

        update_best_lower_bound();

        if (best_bound() > best_obj()) {
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

    m_nodes_manager->revert_local_changes(m_relaxations.get().model());

    //call_callback(Event_::Algorithm::End);

}

void BranchAndBound::create_root_node() {
    m_nodes_manager->create_root_node();
    ++m_n_created_nodes;
}

void BranchAndBound::solve_queued_nodes() {

    while (
            m_nodes_manager->has_node_to_be_processed()
            && !is_terminated()
            ) {

        m_nodes_manager->set_current_node_to_next_node_to_be_processed();

        idol_Log(Trace,
                 BranchAndBound,
                 "Current node is now node " << m_nodes_manager->current_node().id() << '.'
        );

        prepare_to_solve_current_node();
        solve_current_node();

        log_node(Debug, m_nodes_manager->current_node());

        analyze_current_node();

        if (m_nodes_manager->has_current_node()) {
            apply_heuristics_on_current_node();
            add_current_node_to_active_nodes();
        }

    }

}

void BranchAndBound::prepare_to_solve_current_node() {
    m_nodes_manager->apply_current_node_to(m_relaxations.get().model());
}

void BranchAndBound::solve_current_node() {
    auto& lower_bounding_model = m_relaxations.get().model();
    if (lower_bounding_model.get(Attr::Obj::Sense) == Minimize) {
        lower_bounding_model.set(Param::Algorithm::BestBoundStop, std::min(best_obj(), get(Param::Algorithm::BestBoundStop)));
    } else {
        lower_bounding_model.set(Param::Algorithm::BestObjStop, std::max(best_obj(), get(Param::Algorithm::BestObjStop)));
    }

    lower_bounding_model.set(Param::Algorithm::TimeLimit, parent().remaining_time());

    lower_bounding_model.optimize();
    m_nodes_manager->save_current_node_solution(parent(), lower_bounding_model);
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

    } else if(m_iteration % get(Param::BranchAndBound::LogFrequency) == 0) {
        log_node(Info, current_node());
    }

    if (current_node_is_infeasible()) {

        idol_Log(Trace, BranchAndBound, "Pruning node " << m_nodes_manager->current_node().id() << " (infeasible).");
        prune_current_node();
        return;

    }

    if (current_node_was_not_solved_to_optimality()) {

        terminate_for_node_could_not_be_solved_to_optimality();
        return;

    }

    m_iter_lower_bound = current_node().objective_value();

    //call_callback(Event_::Algorithm::NewIterLb);

    if (current_node_has_a_valid_solution()) {

        add_current_node_to_solution_pool();

        idol_Log(Trace, BranchAndBound, "Valid solution found at node " << current_node().id() << '.');

        if (current_node_is_below_upper_bound()) {
            set_current_node_as_incumbent();
            set_best_obj(current_node().objective_value());
            log_node(Info, m_nodes_manager->current_node());
            idol_Log(Trace, BranchAndBound, "Better incumbent found at node " << current_node().id() << ".");

            call_callback(Callback::IncumbentFound);
        }

        reset_current_node();
        return;

    }

    if (current_node_is_above_upper_bound()) {

        idol_Log(Trace, BranchAndBound, "Pruning node " << current_node().id() << " (by bound).");
        prune_current_node();
        return;

    }

}

const Node &BranchAndBound::current_node() const {
    return m_nodes_manager->current_node();
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
    if (status == Feasible) {
        return current_node().reason() != UserObjLimit;
    }
    if (status == Unknown) {
        return current_node().reason() != CutOff;
    }
    return status != Optimal;
}

bool BranchAndBound::current_node_has_a_valid_solution() const {
    return m_nodes_manager->current_node_has_a_valid_solution();
}

void BranchAndBound::set_current_node_as_incumbent() {
    m_nodes_manager->set_current_node_as_incumbent();
}

void BranchAndBound::add_current_node_to_solution_pool() {
    m_nodes_manager->add_current_node_to_solution_pool();
}

void BranchAndBound::reset_current_node() {
    m_nodes_manager->reset_current_node();
}

bool BranchAndBound::current_node_is_below_upper_bound() {
    return current_node().objective_value() < best_obj();
}

bool BranchAndBound::current_node_is_above_upper_bound() {
    const double objective_value = current_node().objective_value();
    return is_pos_inf(objective_value) || objective_value > std::min(get(Param::Algorithm::CutOff), best_obj());
}

void BranchAndBound::apply_heuristics_on_current_node() {
    // call_callback(Event_::BranchAndBound::RelaxationSolved);
    call_callback(Callback::NodeSolved);
}

void BranchAndBound::prune_current_node() {
    m_nodes_manager->prune_current_node();
}

void BranchAndBound::add_current_node_to_active_nodes() {
    m_nodes_manager->add_current_node_to_active_nodes();
}

void BranchAndBound::prune_active_nodes_by_bound() {
    m_nodes_manager->active_nodes().prune_by_bound(std::min(get(Param::Algorithm::CutOff), best_obj()));
}

void BranchAndBound::update_best_lower_bound() {

    if (m_nodes_manager->active_nodes().empty()) {
        set_best_bound(best_obj());
        // call_callback(Event_::Algorithm::NewBestLb);
        return;
    }

    const auto& lowest_node = m_nodes_manager->active_nodes().lowest_node();
    if (double lb = lowest_node.objective_value() ; lb > best_bound()) {
        set_best_bound(lb);
        log_node(Info, lowest_node);
        // call_callback(Event_::Algorithm::NewBestLb);
    }

}

bool BranchAndBound::no_active_nodes() {
    return m_nodes_manager->active_nodes().empty();
}

void BranchAndBound::branch() {

    if (is_terminated()) { return; }

    m_nodes_manager->active_nodes().select_node_for_branching();

    m_n_created_nodes += m_nodes_manager->create_child_nodes();

    m_nodes_manager->active_nodes().remove_node_selected_for_branching();
}

bool BranchAndBound::iteration_limit_is_reached() const {
    return m_iteration >= get(Param::Algorithm::MaxIterations);
}

bool BranchAndBound::time_limit_is_reached() const {
    return get(Param::Algorithm::TimeLimit) <= parent().time().count();
}

void BranchAndBound::terminate_for_no_active_nodes() {
    idol_Log(Trace, BranchAndBound, "Terminate. No active node.");
    set_status(m_nodes_manager->has_incumbent() ? Optimal : Infeasible);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_gap_is_closed() {
    idol_Log(Trace, BranchAndBound, "Terminate. Gap is closed.");
    set_status(Optimal);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_infeasibility() {
    idol_Log(Trace, BranchAndBound, "Terminate. Infeasibility detected.");
    set_status(Infeasible);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_unboundedness() {
    set_best_obj(-Inf);
    idol_Log(Trace, BranchAndBound, "Terminate. Unboundedness detected.");
    set_status(Unbounded);
    set_reason(Proved);
    terminate();
}

void BranchAndBound::terminate_for_node_could_not_be_solved_to_optimality() {
    idol_Log(Trace, BranchAndBound, "Terminate. Current node could node be solved to optimality (node " << current_node().id() << ").");
    set_status(Fail);
    set_reason(NotSpecified);
    terminate();
}

void BranchAndBound::terminate_for_iteration_limit_is_reached() {
    idol_Log(Trace, BranchAndBound, "Terminate. The maximum number of iterations has been reached.")
    set_status(m_nodes_manager->has_incumbent() ? Feasible : Infeasible);
    set_reason(IterationCount);
    terminate();
}

void BranchAndBound::terminate_for_error_lb_greater_than_ub() {
    idol_Log(Trace, BranchAndBound, "Terminate. Best LB > Best UB.");
    set_status(Fail);
    set_reason(NotSpecified);
    terminate();
}

void BranchAndBound::terminate_for_time_limit_is_reached() {
    if (is_terminated()) { return; }
    idol_Log(Trace, BranchAndBound, "Terminate. Time limit is reached.");
    set_status(m_nodes_manager->has_incumbent() ? Feasible : Infeasible);
    set_reason(TimeLimit);
    terminate();
}

void BranchAndBound::log_node(LogLevel t_msg_level, const Node &t_node) const {

    const double objective_value = t_node.objective_value();
    const unsigned int id = t_node.id();
    char sign = ' ';

    if (equals(objective_value, best_obj(), ToleranceForAbsoluteGapMIP)) {
        sign = '-';
    } else if (equals(objective_value, best_bound(), ToleranceForAbsoluteGapMIP)) {
        sign = '+';
    }

    idol_Log(t_msg_level,
             BranchAndBound,
             "<Node=" << (id == -1 ? "H" : std::to_string(id)) << sign << "> "
                      << "<Iter=" << m_iteration << "> "
                      << "<Time=" << parent().time().count() << "> "
                      << "<Levl=" << t_node.level() << "> "
                      << "<Unex=" << m_nodes_manager->active_nodes().size() << "> "
                      << "<Stat=" << (SolutionStatus) t_node.status() << "> "
                      << "<Reas=" << (SolutionReason) t_node.reason() << "> "
              << "<ObjV=" << t_node.objective_value() << "> "
              << "<Lb=" << best_bound() << "> "
              << "<Ub=" << best_obj() << "> "
              << "<RGap=" << get(Attr::Solution::RelGap) * 100 << "> "
              << "<AGap=" << get(Attr::Solution::AbsGap) << "> "
    );

}

int BranchAndBound::get(const Parameter<int> &t_param) const {

    if (t_param.is_in_section(Param::Sections::BranchAndBound)) {
        return m_int_parameters.get(t_param);
    }

    return Algorithm::get(t_param);
}

void BranchAndBound::set(const Parameter<int> &t_param, int t_value) {

    if (t_param.is_in_section(Param::Sections::BranchAndBound)) {
        m_int_parameters.set(t_param, t_value);
        return;
    }

    m_relaxations.get().model().set(t_param, t_value);
}

double BranchAndBound::get(const Req<double, Var> &t_attr, const Var &t_var) const {

    if (t_attr == Attr::Solution::Primal) {
        if (m_nodes_manager->has_incumbent()) {
            return m_nodes_manager->incumbent().primal_solution().get(t_var);
        } else {
            throw Exception("No incumbent found.");
        }
    }

    return Base::get(t_attr, t_var);
}

double BranchAndBound::get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const {
    return m_relaxations.get().model().get(t_attr, t_ctr);
}

void BranchAndBound::set(const Parameter<bool> &t_param, bool t_value) {
    m_relaxations.get().model().set(t_param, t_value);
}

void BranchAndBound::set(const Parameter<double> &t_param, double t_value) {
    m_relaxations.get().model().set(t_param, t_value);
}

const Expr<Var, Var> &BranchAndBound::get(const Req<Expr<Var, Var>, void> &t_attr) const {
    return m_relaxations.get().model().get(t_attr);
}

void BranchAndBound::set(const Req<Expr<Var, Var>, void> &t_attr, Expr<Var, Var> &&t_value) {
    m_relaxations.get().model().set(t_attr, std::move(t_value));
}

void BranchAndBound::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {
    m_relaxations.get().model().set(t_attr, t_var, t_value);
}

void BranchAndBound::set(const Req<Constant, Ctr> &t_attr, const Ctr &t_ctr, Constant &&t_value) {
    m_relaxations.get().model().set(t_attr, t_ctr, std::move(t_value));
}

void BranchAndBound::call_callback(Callback::Event t_event) {
    if (!m_callback) { return; }
    m_callback->execute(t_event);
}

const AbstractModel &BranchAndBound::relaxed_model() const {
    return m_relaxations.get().model();
}

void BranchAndBound::submit_solution(Solution::Primal t_solution) {
    m_nodes_manager->submit_solution(std::move(t_solution), best_obj());
}
