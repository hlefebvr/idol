//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONSUBPROBLEM_H
#define OPTIMIZE_COLUMNGENERATIONSUBPROBLEM_H

#include "AbstractColumnGenerationSubproblem.h"
#include "algorithms/solution-strategies/AbstractSolutionStrategy.h"
#include "algorithms/solution-strategies/decomposition/generators/AbstractGenerator.h"
#include "algorithms/solution-strategies/column-generation/generators/ColumnGenerator.h"
#include "algorithms/logs/Log.h"
#include <memory>
#include <list>
#include <iomanip>
#include <functional>

class Model;

class ColumnGenerationSubProblem : public AbstractColumnGenerationSubproblem {
    AbstractSolutionStrategy& m_rmp_strategy;
    std::unique_ptr<AbstractSolutionStrategy> m_exact_solution_strategy;
    std::unique_ptr<AbstractColumnGenerator> m_generator;
    std::list<std::unique_ptr<Solution::Primal>> m_primal_solutions;

    using PresentColumnList = std::list<std::pair<Var, Solution::Primal&>>;

    PresentColumnList m_currently_present_variables;
public:
    explicit ColumnGenerationSubProblem(AbstractSolutionStrategy& t_rmp_strategy);

    void build() override;

    void solve() override;

    using PresentColumns = ConstIteratorForward<PresentColumnList>;

    [[nodiscard]] PresentColumns currently_present_variables() const { return PresentColumns(m_currently_present_variables); }

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    [[nodiscard]] Row get_pricing_objective(const Solution::Dual &t_duals) const override;

    void update_pricing_objective(const Row& t_objective) override;

    void log_last_primal_solution() const override;

    void save_last_primal_solution() override;

    [[nodiscard]] bool is_unbounded() const override;

    [[nodiscard]] bool is_infeasible() const override;

    [[nodiscard]] bool could_not_be_solved_to_optimality() const override;

    [[nodiscard]] bool improving_column_found() const override;

    [[nodiscard]] TempVar create_column_from(const Solution::Primal& t_primal_solution) const;

    void add_column_to_rmp() override;

    bool set_lower_bound(const Var &t_var, double t_lb) override;

    bool set_upper_bound(const Var &t_var, double t_ub) override;

    AbstractSolutionStrategy& exact_solution_strategy() { return *m_exact_solution_strategy; }

    AbstractSolutionStrategy& rmp_solution_strategy() { return m_rmp_strategy; }

    void remove_column_if(const std::function<bool(const Var&, const Solution::Primal&)>& t_indicator_for_removal);

    std::optional<Ctr> contribute_to_add_constraint(TempCtr &t_temporay_constraint) override;

    bool update_constraint_rhs(const Ctr &t_ctr, double t_rhs) override;

    bool remove_constraint(const Ctr& t_ctr) override;

    template<class T, class ...Args> T& set_solution_strategy(Args&& ...t_args);

    template<class T, class ...Args> T& set_generation_strategy(Args&& ...t_args);
};

template<class T, class... Args>
T &ColumnGenerationSubProblem::set_solution_strategy(Args &&... t_args) {
    auto* exact_solution_strategy = new T(std::forward<Args>(t_args)...);
    m_exact_solution_strategy.reset(exact_solution_strategy);
    return *exact_solution_strategy;
}

template<class T, class... Args>
T &ColumnGenerationSubProblem::set_generation_strategy(Args &&... t_args) {
    auto* generator = new T(std::forward<Args>(t_args)...);
    m_generator.reset(generator);
    return *generator;
}

#endif //OPTIMIZE_COLUMNGENERATIONSUBPROBLEM_H
