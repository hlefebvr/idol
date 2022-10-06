//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITION_H
#define OPTIMIZE_DECOMPOSITION_H

#include "algorithms/external-solver/ExternalSolver.h"
#include "algorithms/decomposition/generation-strategies/AbstractGenerationStrategy.h"
#include "DecompositionId.h"
#include <functional>

class Decomposition : public Algorithm {
    std::unique_ptr<Algorithm> m_rmp_strategy;
    std::list<std::unique_ptr<AbstractGenerationStrategy>> m_generation_strategies;

    Attributes<> m_attributes;
protected:
    AbstractAttributes &attributes() override { return m_attributes; }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
public:
    Decomposition() = default;

    void build() override;

    void solve() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;

    void update_constraint_rhs(const Ctr &t_ctr, double t_rhs) override;

    Ctr add_constraint(TempCtr t_temporary_constraint) override;

    void remove_constraint(const Ctr &t_constraint) override;

    template<class T, class ...Args> T& set_rmp_solution_strategy(Args&& ...t_args) {
        auto* solution_strategy = new T(std::forward<Args>(t_args)...);
        m_rmp_strategy.reset(solution_strategy);
        m_rmp_strategy->set<Attr::InfeasibleOrUnboundedInfo>(true);
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

#endif //OPTIMIZE_DECOMPOSITION_H
