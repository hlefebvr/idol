//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_DECOMPOSITION_H
#define OPTIMIZE_DECOMPOSITION_H

#include "GenerationAlgorithmWithAttributes.h"
#include "../attributes/Attributes.h"
#include "../attributes/Attributes_Base.h"
#include "DecompositionId.h"
#include <functional>
#include <memory>

class Decomposition : public AlgorithmWithAttributes<> {
    std::unique_ptr<Algorithm> m_rmp_strategy;
    std::list<std::unique_ptr<Algorithm>> m_generation_strategies;
protected:
    void execute() override;
public:

    Decomposition() = default;

    void build() override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    void update_lb(const Var &t_var, double t_lb) override;

    void update_ub(const Var &t_var, double t_ub) override;

    void update_coefficient_rhs(const Ctr &t_ctr, double t_rhs) override;

    Ctr add_row(TempCtr t_temporary_constraint) override;

    void remove(const Ctr &t_constraint) override;

    template<class T, class ...Args> T& set_rmp_solution_strategy(Args&& ...t_args) {
        auto* solution_strategy = new T(std::forward<Args>(t_args)...);
        m_rmp_strategy.reset(solution_strategy);
        m_rmp_strategy->set<Attr::InfeasibleOrUnboundedInfo>(true);
        return *solution_strategy;
    }

    template<class T, class ...Args> T& add_generation_strategy(Args&& ...t_args) {

        auto* rmp_strategy = m_generation_strategies.empty() ? m_rmp_strategy.get() : m_generation_strategies.back().get();

        auto* generation_strategy = new T(DecompositionId(*rmp_strategy), std::forward<Args>(t_args)...);

        m_generation_strategies.emplace_back(generation_strategy);

        return *generation_strategy;
    }
};

#endif //OPTIMIZE_DECOMPOSITION_H
