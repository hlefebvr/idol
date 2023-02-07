//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_NODES_BASIC_H
#define OPTIMIZE_NODES_BASIC_H

#include "Node.h"
#include "modeling/solutions/Solution.h"
#include "containers/Map.h"
#include "modeling/variables/Var.h"
#include <optional>

namespace Nodes {
    class Basic;
}

class Nodes::Basic : public Node {
    Solution::Primal m_primal_solutions;
    Map<Var, double> m_local_lower_bounds;
    Map<Var, double> m_local_upper_bounds;
    unsigned int m_level = 0;

protected:
    Basic(unsigned int t_id, const Basic& t_src);
    static std::optional<double> get_local_bound(const Var& t_var, const Map<Var, double>& t_local_bounds);
public:
    explicit Basic(unsigned int t_id) : Node(t_id) {}

    void save_solution(const AbstractModel& t_strategy) override;

    void set_solution(Solution::Primal&& t_solution);

    [[nodiscard]] const Map<Var, double>& local_lower_bounds() const { return m_local_lower_bounds; }
    [[nodiscard]] const Map<Var, double>& local_upper_bounds() const { return m_local_upper_bounds; }

    [[nodiscard]] std::optional<double> get_local_lower_bound(const Var& t_var) const { return get_local_bound(t_var, m_local_lower_bounds); }
    [[nodiscard]] std::optional<double> get_local_upper_bound(const Var& t_var) const { return get_local_bound(t_var, m_local_upper_bounds); }

    [[nodiscard]] int status() const override { return m_primal_solutions.status(); }
    [[nodiscard]] int reason() const override { return m_primal_solutions.reason(); }
    [[nodiscard]] double objective_value() const override { return m_primal_solutions.objective_value(); }
    [[nodiscard]] const Solution::Primal& primal_solution() const override { return m_primal_solutions; }
    [[nodiscard]] const Solution::Dual& dual_solution() const override { throw Exception("Not available."); };
    [[nodiscard]] unsigned int level() const override { return m_level; }

    [[nodiscard]] Basic *create_child(unsigned int t_id) const override;
    void set_local_lower_bound(const Var &t_var, double t_lb) override;
    void set_local_upper_bound(const Var &t_var, double t_ub) override;
};


#endif //OPTIMIZE_NODES_BASIC_H
