//
// Created by henri on 23.11.23.
//

#ifndef IDOL_INFO_H
#define IDOL_INFO_H

#include "LoggerFactory.h"

namespace idol {
    template<class NodeInfoT> class LogInfo;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::LogInfo : public LoggerFactory<NodeInfoT> {

    static constexpr auto double_precision = 5;
    static constexpr auto n_sections = 3;

    static constexpr auto log_width_explored = 6;
    static constexpr auto log_width_open = 6;
    static constexpr auto log_width_total_time = 12;

    static constexpr auto log_width_current_node_depth = 6;
    static constexpr auto log_width_problem_best_bound = 10;
    static constexpr auto log_width_problem_best_obj = 10;
    static constexpr auto log_width_problem_rel_gap = 10;
    static constexpr auto log_width_problem_abs_gap = 10;

    static constexpr auto log_width_current_node_status = 12;
    static constexpr auto log_width_current_node_reason = 12;
    static constexpr auto log_width_current_node_obj = 10;

    static constexpr auto log_section_width_algorithm = (log_width_explored + log_width_open + log_width_total_time);
    static constexpr auto log_section_width_problem = (log_width_problem_best_bound + log_width_problem_best_obj + log_width_problem_rel_gap + log_width_problem_abs_gap);
    static constexpr auto current_node_space = (log_width_current_node_status + log_width_current_node_reason + log_width_current_node_obj + log_width_current_node_depth);
    static constexpr auto table_space = log_section_width_algorithm + log_section_width_problem + current_node_space + n_sections * 3 + 1;


    std::optional<double> m_log_frequency_in_seconds;
public:
    class Strategy : public LoggerFactory<NodeInfoT>::Strategy {
        double m_last_log_timestamp = 0;
        double m_frequency_in_seconds;
        bool m_header_has_been_printed = false;
        bool m_root_node_has_been_printed = false;
        bool m_log_after_root_node = false;
    protected:
        void log_header();
    public:

        Strategy(Optimizers::BranchAndBound<NodeInfoT>& t_parent, double t_frequency_in_seconds);

        void initialize() override;

        void log_node_after_solve(const Node <NodeInfoT> &t_node) override;

        void log_root_node(const Node<NodeInfoT>& t_node);

        void log_after_termination() override;
    };

    Strategy *operator()(Optimizers::BranchAndBound<NodeInfoT>& t_parent) const override;

    LoggerFactory<NodeInfoT> *clone() const override;

    LogInfo& with_frequency_in_seconds(double t_frequency);
};

template<class NodeInfoT>
idol::LogInfo<NodeInfoT> &idol::LogInfo<NodeInfoT>::with_frequency_in_seconds(double t_frequency) {

    if (m_log_frequency_in_seconds.has_value()) {
        throw Exception("Log frequency has already been configured.");
    }

    m_log_frequency_in_seconds = t_frequency;

    return *this;
}

template<class NodeInfoT>
typename idol::LogInfo<NodeInfoT>::Strategy *idol::LogInfo<NodeInfoT>::operator()(Optimizers::BranchAndBound<NodeInfoT>& t_parent) const {
    return new Strategy(t_parent, m_log_frequency_in_seconds.has_value() ? m_log_frequency_in_seconds.value() : 5);
}

template<class NodeInfoT>
idol::LoggerFactory<NodeInfoT> *idol::LogInfo<NodeInfoT>::clone() const {
    return new LogInfo<NodeInfoT>(*this);
}

template<class NodeInfoT>
idol::LogInfo<NodeInfoT>::Strategy::Strategy(Optimizers::BranchAndBound<NodeInfoT> &t_parent, double t_frequency_in_seconds)
        : LoggerFactory<NodeInfoT>::Strategy(t_parent),
          m_frequency_in_seconds(t_frequency_in_seconds) {

}

template<class NodeInfoT>
void idol::LogInfo<NodeInfoT>::Strategy::initialize() {
    LoggerFactory<NodeInfoT>::Strategy::initialize();

    m_last_log_timestamp = 0;
    m_header_has_been_printed = false;
    m_root_node_has_been_printed = false;

    std::cout << "Solving root node with " << this->parent().relaxation().optimizer().name() << "...\n" << std::endl;

}

template<class NodeInfoT>
void idol::LogInfo<NodeInfoT>::Strategy::log_node_after_solve(const Node <NodeInfoT> &t_node) {
    LoggerFactory<NodeInfoT>::Strategy::Strategy::log_node_after_solve(t_node);

    const auto& parent = this->parent();

    const double total_time = parent.time().count();

    if (!m_root_node_has_been_printed) {
        log_root_node(t_node);
        return;
    }

    if (total_time - m_last_log_timestamp < m_frequency_in_seconds) {
        return;
    }

    log_header();

    m_last_log_timestamp = total_time;

    std::cout << " | ";
    std::cout << std::setw(log_width_explored) << parent.n_solved_nodes();
    std::cout << std::setw(log_width_open) << "?";
    std::cout << std::setw(log_width_total_time) << parent.time().count();

    // Problem
    std::cout << " | ";
    std::cout << std::setw(log_width_problem_best_bound) << pretty_double(parent.get_best_bound(), double_precision);
    std::cout << std::setw(log_width_problem_best_obj) << pretty_double(parent.get_best_obj(), double_precision);
    std::cout << std::setw(log_width_problem_rel_gap) << pretty_double(parent.get_relative_gap() * 100, double_precision);
    std::cout << std::setw(log_width_problem_abs_gap) << pretty_double(parent.get_absolute_gap(), double_precision);

    // Current Node
    std::cout << " | ";
    std::cout << std::setw(log_width_current_node_depth) << t_node.level();
    std::cout << std::setw(log_width_current_node_status) << t_node.info().status();
    std::cout << std::setw(log_width_current_node_reason) << t_node.info().reason();
    std::cout << std::setw(log_width_current_node_obj) << pretty_double(t_node.info().objective_value(), double_precision);
    std::cout << " | ";

    std::cout << std::endl;

}

template<class NodeInfoT>
void idol::LogInfo<NodeInfoT>::Strategy::log_root_node(const Node<NodeInfoT> &t_node) {

    auto& branch_and_bound = this->parent();
    const double total_time = branch_and_bound.time().count();

    std::cout
            << "Root relaxation: objective " << t_node.info().objective_value()
            << ", " << total_time << " seconds\n"
            << std::endl;

    if (!m_log_after_root_node) {
        branch_and_bound.relaxation().optimizer().set_param_logs(false);
    }

    m_root_node_has_been_printed = true;

}

template<class NodeInfoT>
void idol::LogInfo<NodeInfoT>::Strategy::log_header() {

    if (m_header_has_been_printed) {
        return;
    }

    std::cout << ' ';center(std::cout, "*", table_space, '*') << '\n';
    std::cout << ' ';center(std::cout, " Branch-and-Bound Algorithm ", table_space) << '\n';
    std::cout << ' ';center(std::cout, "*", table_space, '*') << '\n';

    std::cout << " | ";
    center(std::cout, "Algorithm", log_section_width_algorithm);std::cout << " | ";
    center(std::cout, "Problem", log_section_width_problem);std::cout << " | ";
    center(std::cout, "Current Node", current_node_space);std::cout << " | ";
    std::cout << '\n';

    std::cout << " | ";
    std::cout << std::setw(log_width_explored) << "Expl.";
    std::cout << std::setw(log_width_open) << "Open";
    std::cout << std::setw(log_width_total_time) << "Time";

    // Problem
    std::cout << " | ";
    std::cout << std::setw(log_width_problem_best_bound) << "Bound";
    std::cout << std::setw(log_width_problem_best_obj) << "Obj";
    std::cout << std::setw(log_width_problem_rel_gap) << "Gap (%)";
    std::cout << std::setw(log_width_problem_abs_gap) << "Abs.";

    // Current Node
    std::cout << " | ";
    std::cout << std::setw(log_width_current_node_depth) << "Depth";
    std::cout << std::setw(log_width_current_node_status) << "Status";
    std::cout << std::setw(log_width_current_node_reason) << "Reason";
    std::cout << std::setw(log_width_current_node_obj) << "Value";
    std::cout << " | ";

    std::cout << std::endl;

    m_header_has_been_printed = true;
}

template<class NodeInfoT>
void idol::LogInfo<NodeInfoT>::Strategy::log_after_termination() {

    LoggerFactory<NodeInfoT>::Strategy::Strategy::log_after_termination();

    auto& parent = this->parent();

    std::cout << "Explored " << parent.n_solved_nodes() << " nodes in " << parent.time().count() << " seconds\n" << std::endl;

    const unsigned int n_solutions = parent.get_n_solutions();

    std::cout << "Solution count " << n_solutions << ":";

    for (unsigned int i = 0 ; i < n_solutions ; ++i) {
        parent.set_solution_index(i);
        std::cout << '\t' << parent.get_best_obj();
    }

    parent.set_solution_index(0);

    std::cout << std::endl;

    const auto status = parent.get_status();
    const auto reason = parent.get_reason();

    switch (reason) {
        case TimeLimit:
            std::cout << "Time limit was reached." << std::endl;
            break;
        case IterLimit:
            std::cout << "Iteration limit was reached." << std::endl;
            break;
        case ObjLimit:
            std::cout << "Objective limit was reached." << std::endl;
            break;
        case Numerical:
            std::cout << "There was unrecoverable numerical troubles during the execution of the algorithm." << std::endl;
            break;
        case NotSpecified:
            std::cout << "The reason for termination is not specified." << std::endl;
            break;
        default:;
    }

    switch (status) {
        case Optimal:
            std::cout << "Optimal solution found (tolerance " << parent.get_tol_mip_relative_gap() << ")" << std::endl;
            break;
        case Feasible:
            std::cout << "Feasible solution found" << std::endl;
            break;
        case Infeasible:
            std::cout << "No feasible solution found" << std::endl;
            break;
        case InfOrUnbnd:
            std::cout << "Problem is infeasible or unbounded" << std::endl;
            break;
        case Unbounded:
            std::cout << "Problem is unbounded" << std::endl;
            break;
        case Fail:
            std::cout << "There was unrecoverable errors." << std::endl;
            break;
        case SubOptimal:
            std::cout << "Sub-optimal solution found" << std::endl;
            break;
        default:;
    }

    std::cout
            << "Best objective " << pretty_double(parent.get_best_obj(), double_precision)
            << ", best bound " << pretty_double(parent.get_best_bound(), double_precision)
            << ", gap " << pretty_double(parent.get_relative_gap() * 100, double_precision) << " %"
            << std::endl;

}

#endif //IDOL_INFO_H
