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

public:
    class Strategy : public LoggerFactory<NodeInfoT>::Strategy {
        double m_last_log_timestamp = 0;
        double m_frequency_in_milliseconds = 50;
        bool m_header_has_been_printed = false;
        bool m_root_node_has_been_printed = false;
        bool m_log_after_root_node = false;
    public:
        Strategy(Optimizers::BranchAndBound<NodeInfoT>& t_parent);

        void initialize() override;

        void log_node_after_solve(const Node <NodeInfoT> &t_node) override;

        void log_root_node(const Node<NodeInfoT>& t_node);

        void log_header();
    };

    Strategy *operator()(Optimizers::BranchAndBound<NodeInfoT>& t_parent) const override;

    LoggerFactory<NodeInfoT> *clone() const override;
};

template<class NodeInfoT>
typename idol::LogInfo<NodeInfoT>::Strategy *idol::LogInfo<NodeInfoT>::operator()(Optimizers::BranchAndBound<NodeInfoT>& t_parent) const {
    return new Strategy(t_parent);
}

template<class NodeInfoT>
idol::LoggerFactory<NodeInfoT> *idol::LogInfo<NodeInfoT>::clone() const {
    return new LogInfo<NodeInfoT>(*this);
}

template<class NodeInfoT>
idol::LogInfo<NodeInfoT>::Strategy::Strategy(Optimizers::BranchAndBound<NodeInfoT> &t_parent)
        : LoggerFactory<NodeInfoT>::Strategy(t_parent) {

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

    const double total_time = parent.time().count(Timer::Milliseconds);

    if (!m_root_node_has_been_printed) {
        log_root_node(t_node);
        return;
    }

    if (total_time - m_last_log_timestamp < m_frequency_in_milliseconds) {
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
    std::cout << std::setw(log_width_problem_rel_gap) << pretty_double(parent.get_relative_gap(), double_precision);
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

#endif //IDOL_INFO_H
