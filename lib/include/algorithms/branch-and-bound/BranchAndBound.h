//
// Created by henri on 13/09/22.
//

#ifndef OPTIMIZE_BRANCHANDBOUND_H
#define OPTIMIZE_BRANCHANDBOUND_H

#include "Node.h"
#include "../Algorithm.h"
#include "../../modeling/numericals.h"
#include "../parameters/Log.h"
#include "NodeStrategy.h"
#include "Attributes_BranchAndBound.h"
#include <vector>
#include <list>
#include <memory>

class Callback;

class BranchAndBound : public Algorithm {
    unsigned int m_n_created_nodes = 0;
    unsigned int m_iteration = 0;
    bool m_is_terminated = false;

    double m_best_lower_bound = -Inf;
    double m_best_upper_bound = +Inf;

    // User strategies
    std::unique_ptr<NodeStrategy> m_nodes;
    std::unique_ptr<Algorithm> m_solution_strategy;
    std::list<std::unique_ptr<::Callback>> m_callbacks;

    Param::BranchAndBound::values<int> m_params_int;
protected:
    bool set_parameter_int(const Parameter<int> &t_param, int t_value) override;

    std::optional<int> get_parameter_int(const Parameter<int> &t_param) const override;

private:
    void initialize();
    void create_root_node();
    void solve_queued_nodes();
    void prepare_node_solution();
    void solve_current_node();
    void analyze_current_node();
    [[nodiscard]] bool current_node_is_root_node() const;
    [[nodiscard]] bool current_node_is_infeasible() const;
    [[nodiscard]] bool current_node_is_unbounded() const;
    [[nodiscard]] bool current_node_was_not_solved_to_optimality() const;
    [[nodiscard]] bool current_node_has_a_valid_solution() const;
    void set_current_node_as_incumbent();
    void add_current_node_to_solution_pool();
    void reset_current_node();
    bool current_node_is_below_upper_bound();
    bool current_node_is_above_upper_bound();
    void apply_heuristics_on_current_node();
    void prune_current_node();
    void add_current_node_to_active_nodes();
    void prune_active_nodes_by_bound();
    void update_best_lower_bound();
    bool no_active_nodes();
    void branch();
    [[nodiscard]] bool iteration_limit_is_reached() const;
    [[nodiscard]] bool time_limit_is_reached() const;

    void terminate();
    void terminate_for_no_active_nodes();
    void terminate_for_gap_is_closed();
    void terminate_for_infeasibility();
    void terminate_for_unboundedness();
    void terminate_for_node_could_not_be_solved_to_optimality();
    void terminate_for_iteration_limit_is_reached();
    void terminate_for_error_lb_greater_than_ub();
    void terminate_for_time_limit_is_reached();

    [[nodiscard]] bool gap_is_closed() const { return relative_gap() <= ToleranceForRelativeGapMIP || absolute_gap() <= ToleranceForAbsoluteGapMIP; }

    void log_node(LogLevel t_msg_level, const Node& t_node) const;

    [[nodiscard]] const Node& current_node() const;

    void execute() override;
public:
    BranchAndBound() = default;

    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }

    Algorithm& solution_strategy() { return *m_solution_strategy; }
    [[nodiscard]] const Algorithm& solution_strategy() const { return *m_solution_strategy; }

    NodeStrategy& node_strategy() { return *m_nodes; }
    [[nodiscard]] const NodeStrategy& node_strategy() const { return *m_nodes; }

    template<class T, class ...Args> T& set_solution_strategy(Args&& ...t_args);

    template<class T, class ...Args> T& set_node_strategy(Args&& ...t_args);

    [[nodiscard]] double lower_bound() const;
    [[nodiscard]] double upper_bound() const;
    [[nodiscard]] double relative_gap() const;
    [[nodiscard]] double absolute_gap() const;

    [[nodiscard]] unsigned int n_created_nodes() const { return m_n_created_nodes; }

    [[nodiscard]] double objective_value() const;
    [[nodiscard]] Solution::Primal primal_solution() const override;

    double get_lb(const Var &t_var) const override;

    double get_ub(const Var &t_var) const override;

    int get_type(const Var &t_var) const override;

    const Column &get_column(const Var &t_var) const override;

    bool has(const Var &t_var) const override;

    const Row &get_row(const Ctr &t_ctr) const override;

    int get_type(const Ctr &t_ctr) const override;

    bool has(const Ctr &t_ctr) const override;

    bool submit_solution(Solution::Primal&& t_solution);

    template<class T, class ...ArgsT> T& add_callback(ArgsT&& ...t_args);

    class Callback;
};

template<class T, class... Args>
T& BranchAndBound::set_solution_strategy(Args &&... t_args) {
    static_assert(std::is_base_of_v<Algorithm, T>);
    auto* solution_strategy = new T(std::forward<Args>(t_args)...);
    m_solution_strategy.reset(solution_strategy);
    return *solution_strategy;
}

template<class T, class... Args>
T &BranchAndBound::set_node_strategy(Args &&... t_args) {
    auto* node_strategy = new T(*this, std::forward<Args>(t_args)...);
    m_nodes.reset(node_strategy);
    return *node_strategy;
}

template<class T, class... ArgsT>
T &BranchAndBound::add_callback(ArgsT&& ...t_args) {
    auto cb = new T(std::forward<ArgsT>(t_args)...);
    m_callbacks.template emplace_back(cb);
    return *cb;
}

#include "../callbacks/BranchAndBoundCallback.h"

#endif //OPTIMIZE_BRANCHANDBOUND_H
