//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITIONSTRATEGY_H
#define OPTIMIZE_DECOMPOSITIONSTRATEGY_H

#include "../branch-and-bound/ExternalSolverStrategy.h"
#include "AbstractGenerationStrategy.h"
#include "DecompositionId.h"
#include <functional>

template<class SolverRMPT>
class DecompositionStrategy : public AbstractSolutionStrategy {
    ExternalSolverStrategy<SolverRMPT> m_rmp_strategy;
    std::list<std::unique_ptr<AbstractGenerationStrategy>> m_generation_strategies;
public:
    explicit DecompositionStrategy(Model& t_rmp_model) : m_rmp_strategy(t_rmp_model) {}

    void build() override;

    void solve() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;

    template<class T, class ...Args> T& add_generation_strategy(Args&& ...t_args) {

        static_assert(std::is_base_of_v<AbstractGenerationStrategy, T>);

        Model* rmp_model = &m_rmp_strategy.model();
        AbstractSolutionStrategy* rmp_strategy = m_generation_strategies.empty() ? (AbstractSolutionStrategy*) &m_rmp_strategy : m_generation_strategies.back().get();

        m_generation_strategies.emplace_back(
                std::make_unique<T>(
                        DecompositionId(*rmp_model, *rmp_strategy),
                        std::forward<Args>(t_args)...
                )
            );

        return dynamic_cast<T&>(*m_generation_strategies.back());
    }
};

template<class SolverRMPT>
void DecompositionStrategy<SolverRMPT>::build() {
    m_rmp_strategy.build();

    if (m_generation_strategies.empty()) {
        throw std::runtime_error("No generation strategy was given to decomposition strategy.");
    }

    for (auto& ptr_to_generator_strategy : m_generation_strategies) {
        ptr_to_generator_strategy->build();
    }
}

template<class SolverRMPT>
void DecompositionStrategy<SolverRMPT>::solve() {
    m_generation_strategies.front()->solve();
}

template<class SolverRMPT>
Solution::Primal DecompositionStrategy<SolverRMPT>::primal_solution() const {
    return m_generation_strategies.front()->primal_solution();
}

template<class SolverRMPT>
Solution::Dual DecompositionStrategy<SolverRMPT>::dual_solution() const {
    return m_generation_strategies.front()->dual_solution();
}

template<class SolverRMPT>
void DecompositionStrategy<SolverRMPT>::set_lower_bound(const Var &t_var, double t_lb) {
    for (auto& ptr_to_decomposition_strategy : m_generation_strategies) {
        ptr_to_decomposition_strategy->set_lower_bound(t_var, t_lb);
    }
}

template<class SolverRMPT>
void DecompositionStrategy<SolverRMPT>::set_upper_bound(const Var &t_var, double t_ub) {
    for (auto& ptr_to_decomposition_strategy : m_generation_strategies) {
        ptr_to_decomposition_strategy->set_upper_bound(t_var, t_ub);
    }
}

#endif //OPTIMIZE_DECOMPOSITIONSTRATEGY_H
