//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONSTRATEGY_H
#define OPTIMIZE_COLUMNGENERATIONSTRATEGY_H

#include "AbstractGenerationStrategy.h"
#include "modeling/models/Model.h"
#include "ColumnGenerationSubproblem.h"

class ColumnGenerationStrategy : public AbstractGenerationStrategy {
    std::list<std::unique_ptr<AbstractColumnGenerationSubproblem>> m_subproblems;
    std::unique_ptr<Solution::Dual> m_last_rmp_duals;

    bool m_is_terminated = false;

    void initialize();
    void save_last_rmp_dual_solution();
    void analyze_last_rmp_dual_solution();
    [[nodiscard]] bool rmp_is_unbounded() const;
    [[nodiscard]] bool rmp_is_infeasible() const;
    [[nodiscard]] bool rmp_could_not_be_solved_to_optimality() const;
    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }
    void save_rmp_farkas();
    void update_subproblems();
    void solve_subproblems();
    void log_last_rmp_dual_solution() const;
    void analyze_last_subproblem_primal_solution(const AbstractColumnGenerationSubproblem& t_subproblem);
    void add_columns();

    void terminate();
    void terminate_for_rmp_is_unbounded();
    void terminate_for_rmp_could_not_be_solved_to_optimality();
    void terminate_for_subproblem_is_infeasible();
    void terminate_for_subproblem_is_unbounded();
    void terminate_for_subproblem_could_not_be_solved_to_optimality();
    void terminate_for_no_improving_column_found();
public:
    explicit ColumnGenerationStrategy(DecompositionId&& t_id);

    void build() override;

    void solve() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;

    template<class T, class ...Args> ColumnGenerationSubProblem<T>& add_subproblem(Args&& ...t_args);
};

template<class T, class... Args>
ColumnGenerationSubProblem<T>& ColumnGenerationStrategy::add_subproblem(Args &&... t_args) {
    m_subproblems.template emplace_back(
            std::make_unique<ColumnGenerationSubProblem<T>>(
                    std::forward<Args>(t_args)...
                )
            );
    return dynamic_cast<ColumnGenerationSubProblem<T>&>(*m_subproblems.back());
}

#endif //OPTIMIZE_COLUMNGENERATIONSTRATEGY_H
