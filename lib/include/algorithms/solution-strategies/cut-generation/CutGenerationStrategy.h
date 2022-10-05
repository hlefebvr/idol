//
// Created by henri on 05/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONSTRATEGY_H
#define OPTIMIZE_CUTGENERATIONSTRATEGY_H

#include "../decomposition/generation-strategies/AbstractGenerationStrategy.h"
#include "algorithms/attributes/Base.h"
#include "algorithms/attributes/Attributes.h"
#include "algorithms/solution-strategies/cut-generation/subproblems/AbstractCutGenerationSubproblem.h"
#include <list>
#include <memory>

class CutGenerationStrategy : public AbstractGenerationStrategy {
    std::list<std::unique_ptr<AbstractCutGenerationSubproblem>> m_subproblems;
    std::unique_ptr<Solution::Primal> m_last_rmp_primals;
    Attributes<AttrType::Base> m_attributes;

    bool m_is_terminated = false;

    void initialize();
    void save_last_rmp_primal_solution();
    void analyze_last_rmp_primal_solution();
    [[nodiscard]] bool rmp_is_unbounded() const;
    [[nodiscard]] bool rmp_is_infeasible() const;
    [[nodiscard]] bool rmp_could_not_be_solved_to_optimality() const;
    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }
    void update_subproblems();
    void solve_subproblems();
    void log_last_rmp_primal_solution() const;
    void analyze_last_subproblem_primal_solution(const AbstractCutGenerationSubproblem& t_subproblem);
    void add_cuts();

    void terminate();
    void terminate_for_rmp_is_infeasible();
    void terminate_for_rmp_could_not_be_solved_to_optimality();
    void terminate_for_no_violated_cut_found();
    void terminate_for_subproblem_is_infeasible();
    void terminate_for_subproblem_could_not_be_solved_to_optimality();
protected:
    AbstractAttributes &attributes() override { return m_attributes; }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
public:
    explicit CutGenerationStrategy(DecompositionId&& t_id);

    void build() override;

    void solve() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    template<class T, class ...Args> T& add_subproblem(Args&& ...t_args);
};

template<class T, class... Args>
T &CutGenerationStrategy::add_subproblem(Args &&... t_args) {
    auto* subproblem = new T(rmp_solution_strategy(), std::forward<Args>(t_args)...);
    m_subproblems.template emplace_back(subproblem);
    return *subproblem;
}

#endif //OPTIMIZE_CUTGENERATIONSTRATEGY_H
