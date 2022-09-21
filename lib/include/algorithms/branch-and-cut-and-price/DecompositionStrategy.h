//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITIONSTRATEGY_H
#define OPTIMIZE_DECOMPOSITIONSTRATEGY_H

#include "../branch-and-bound/ExternalSolverStrategy.h"
#include "AbstractGenerationStrategy.h"
#include "DecompositionId.h"
#include <functional>

class DecompositionStrategy : public AbstractSolutionStrategy {
    std::unique_ptr<AbstractSolutionStrategy> m_rmp_strategy;
    std::list<std::unique_ptr<AbstractGenerationStrategy>> m_generation_strategies;
public:
    DecompositionStrategy() = default;

    void build() override;

    void solve() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;

    template<class T, class ...Args> T& set_rmp_solution_strategy(Args&& ...t_args) {
        auto* solution_strategy = new T(std::forward<Args>(t_args)...);
        m_rmp_strategy.reset(solution_strategy);
        return *solution_strategy;
    }

    template<class T, class ...Args> T& add_generation_strategy(Args&& ...t_args) {

        static_assert(std::is_base_of_v<AbstractGenerationStrategy, T>);

        auto* rmp_strategy = m_generation_strategies.empty() ? m_rmp_strategy.get() : m_generation_strategies.back().get();

        auto* generation_strategy = new T(DecompositionId(*rmp_strategy), std::forward<Args>(t_args)...);

        m_generation_strategies.emplace_back(generation_strategy);

        return *generation_strategy;
    }
};

#endif //OPTIMIZE_DECOMPOSITIONSTRATEGY_H
