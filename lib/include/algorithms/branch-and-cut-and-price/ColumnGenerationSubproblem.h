//
// Created by henri on 16/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONSUBPROBLEM_H
#define OPTIMIZE_COLUMNGENERATIONSUBPROBLEM_H

#include "../branch-and-bound/AbstractSolutionStrategy.h"
#include "AbstractGenerator.h"
#include "ColumnGenerator.h"
#include "../logs/Log.h"
#include <memory>
#include <iomanip>

class Model;

class AbstractColumnGenerationSubproblem : public AbstractSolutionStrategy {
public:
    virtual void log_last_primal_solution() const = 0;
    virtual void save_last_primal_solution() = 0;
    [[nodiscard]] virtual Row get_pricing_objective(const Solution::Dual& t_duals) const = 0;
    virtual void update_pricing_objective(const Row& t_objective) = 0;
    [[nodiscard]] virtual bool is_unbounded() const = 0;
    [[nodiscard]] virtual bool is_infeasible() const = 0;
    [[nodiscard]] virtual bool could_not_be_solved_to_optimality() const = 0;
    [[nodiscard]] virtual bool improving_column_found() const = 0;
    [[nodiscard]] virtual TempVar create_column() const = 0;
};

template<class ExactSolutionStrategyT>
class ColumnGenerationSubProblem : public AbstractColumnGenerationSubproblem {
    ExactSolutionStrategyT m_exact_solution_strategy;
    ColumnGenerator m_generator;
    std::unique_ptr<Solution::Primal> m_last_primal_solution;
public:
    template<class ...Args> explicit ColumnGenerationSubProblem(ColumnGenerator t_generator, Args&& ...t_args);

    void build() override;

    void solve() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    Row get_pricing_objective(const Solution::Dual &t_duals) const override;

    void update_pricing_objective(const Row& t_objective) override;

    void log_last_primal_solution() const override;

    void save_last_primal_solution() override;

    bool is_unbounded() const override;

    bool is_infeasible() const override;

    bool could_not_be_solved_to_optimality() const override;

    bool improving_column_found() const override;

    TempVar create_column() const override;

    ExactSolutionStrategyT& exact_solution_strategy() { return m_exact_solution_strategy; }
};

template<class ExactSolutionStrategyT>
template<class... Args>
ColumnGenerationSubProblem<ExactSolutionStrategyT>::ColumnGenerationSubProblem(ColumnGenerator t_generator, Args &&... t_args)
    : m_generator(std::move(t_generator)),
      m_exact_solution_strategy(std::forward<Args>(t_args)...) {

}

template<class ExactSolutionStrategyT>
void ColumnGenerationSubProblem<ExactSolutionStrategyT>::solve() {
    m_exact_solution_strategy.solve();
}

template<class ExactSolutionStrategyT>
Solution::Primal ColumnGenerationSubProblem<ExactSolutionStrategyT>::primal_solution() const {
    return *m_last_primal_solution;
}

template<class ExactSolutionStrategyT>
Solution::Dual ColumnGenerationSubProblem<ExactSolutionStrategyT>::dual_solution() const {
    return m_exact_solution_strategy.dual_solution();
}

template<class ExactSolutionStrategyT>
Row ColumnGenerationSubProblem<ExactSolutionStrategyT>::get_pricing_objective(const Solution::Dual &t_duals) const {
    // TODO what about objective ?
    Row result;
    for (auto [ctr, row] : m_generator) {
        row *= -t_duals.get(ctr);
        result += row;
    }
    return result;
}

template<class ExactSolutionStrategyT>
void ColumnGenerationSubProblem<ExactSolutionStrategyT>::update_pricing_objective(const Row &t_objective) {
    m_exact_solution_strategy.update_objective(t_objective);
}

template<class ExactSolutionStrategyT>
void ColumnGenerationSubProblem<ExactSolutionStrategyT>::build() {
    m_exact_solution_strategy.build();
}

template<class ExactSolutionStrategyT>
void ColumnGenerationSubProblem<ExactSolutionStrategyT>::save_last_primal_solution() {
    m_last_primal_solution = std::make_unique<Solution::Primal>(m_exact_solution_strategy.primal_solution());
}

template<class ExactSolutionStrategyT>
bool ColumnGenerationSubProblem<ExactSolutionStrategyT>::is_unbounded() const {
    return m_last_primal_solution->status() == Unbounded;
}

template<class ExactSolutionStrategyT>
bool ColumnGenerationSubProblem<ExactSolutionStrategyT>::is_infeasible() const {
    return m_last_primal_solution->status() == Infeasible;
}

template<class ExactSolutionStrategyT>
bool ColumnGenerationSubProblem<ExactSolutionStrategyT>::could_not_be_solved_to_optimality() const {
    return m_last_primal_solution->status() != Optimal;
}

template<class ExactSolutionStrategyT>
void ColumnGenerationSubProblem<ExactSolutionStrategyT>::log_last_primal_solution() const {
    EASY_LOG(Debug, "column-generation",
             std::setw(5)
             << "SP"
             << std::setw(15)
             << m_last_primal_solution->status()
             << std::setw(10)
             << m_last_primal_solution->objective_value()
    );
}

template<class ExactSolutionStrategyT>
bool ColumnGenerationSubProblem<ExactSolutionStrategyT>::improving_column_found() const {
    return m_last_primal_solution->objective_value() < -ToleranceForAbsoluteGapPricing;
}

template<class ExactSolutionStrategyT>
TempVar ColumnGenerationSubProblem<ExactSolutionStrategyT>::create_column() const {
    Column column;

    //column.set_constant(m_last_primal_solution->objective_value());
    // evaluate value from original objective

    for (const auto& [ctr, expr] : m_generator) {
        double coeff = expr.constant().constant();
        for (const auto& [var, value] : expr) {
            coeff += value.constant() * m_last_primal_solution->get(var);
        }
        column.set(ctr, coeff);
    }

    return { m_generator.lb(), m_generator.ub(), m_generator.type(), std::move(column) };
}


#endif //OPTIMIZE_COLUMNGENERATIONSUBPROBLEM_H
