//
// Created by henri on 01/02/23.
//

#ifndef IDOL_BRANCHANDBOUND_H
#define IDOL_BRANCHANDBOUND_H

#include <vector>
#include <functional>

#include "../../modeling/models/Model.h"
#include "../Algorithm.h"
#include "NodeStrategy.h"
#include "backends/parameters/Logs.h"
#include "Attributes_BranchAndBound.h"

class BranchAndBound : public Algorithm {
    const unsigned int m_n_models = 1;
    std::vector<Model> m_lower_bounding_models;

    unsigned int m_n_created_nodes = 0;
    unsigned int m_iteration = 0;

    double m_iter_lower_bound = -Inf;
    double m_iter_upper_bound = +Inf;

    Param::BranchAndBound::values<int> m_int_parameters;

    std::unique_ptr<NodeStrategy> m_nodes_manager;
protected:
    void initialize() override;
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;

    void hook_before_optimize() override;
    void hook_optimize() override;

    [[nodiscard]] const Node& current_node() const;

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

    void terminate_for_no_active_nodes();
    void terminate_for_gap_is_closed();
    void terminate_for_infeasibility();
    void terminate_for_unboundedness();
    void terminate_for_node_could_not_be_solved_to_optimality();
    void terminate_for_iteration_limit_is_reached();
    void terminate_for_error_lb_greater_than_ub();
    void terminate_for_time_limit_is_reached();

    [[nodiscard]] bool gap_is_closed() const { return get(Attr::Solution::RelGap) <= ToleranceForRelativeGapMIP || get(Attr::Solution::AbsGap) <= ToleranceForAbsoluteGapMIP; }

    void log_node(LogLevel t_msg_level, const Node& t_node) const;

    using Algorithm::get;
    using Algorithm::set;

    void set(const Parameter<int>& t_param, int t_value) override;
    int get(const Parameter<int>& t_param) const override;
public:
    explicit BranchAndBound(const Model& t_model);

    template<class T, class ...ArgsT>
    decltype(T::apply(std::declval<Model&>())) relax(ArgsT&& ...t_args);

    template<class T, class ...ArgsT> std::vector<std::reference_wrapper<T>> set_node_backend(ArgsT&& ...t_args);

    template<class T, class ...Args> T& set_nodes_manager(Args&& ...t_args);
};

template<class T, class... ArgsT>
decltype(T::apply(std::declval<Model&>())) BranchAndBound::relax(ArgsT&& ...t_args) {

    if constexpr (std::is_same_v<decltype(T::apply(std::declval<Model&>())), void>) {

        m_lower_bounding_models.emplace_back(parent().clone());

        for (unsigned int i = 1 ; i < m_n_models ; ++i) {
            m_lower_bounding_models.emplace_back(m_lower_bounding_models.front().clone());
        }

    } else {

        m_lower_bounding_models.emplace_back(parent().clone());
        auto result = T::apply(m_lower_bounding_models.back(), std::forward<ArgsT>(t_args)...);

        for (unsigned int i = 1; i < m_n_models; ++i) {
            m_lower_bounding_models.emplace_back(m_lower_bounding_models.front().clone());
        }

        return result;

    }
}

template<class T, class... ArgsT>
std::vector<std::reference_wrapper<T>> BranchAndBound::set_node_backend(ArgsT &&... t_args) {

    std::vector<std::reference_wrapper<T>> result;
    result.reserve(m_lower_bounding_models.size());

    for (auto& model : m_lower_bounding_models) {
        result.emplace_back(model.set_backend<T>(std::forward<ArgsT>(t_args)...));
    }

    return result;
}

template<class T, class... Args>
T &BranchAndBound::set_nodes_manager(Args &&... t_args) {
    auto* result = new T(*this, std::forward<Args>(t_args)...);
    m_nodes_manager.reset(result);
    return *result;
}

#endif //IDOL_BRANCHANDBOUND_H
