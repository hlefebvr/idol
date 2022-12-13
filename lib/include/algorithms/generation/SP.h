//
// Created by henri on 13/12/22.
//

#ifndef IDOL_SP_H
#define IDOL_SP_H

#include <memory>
#include "../Algorithm.h"

template<class SubProblemT> class Generation;

template<class ParentT>
class SP {
    ParentT& m_parent;
    std::unique_ptr<Algorithm> m_exact_solution_strategy;
protected:
    ParentT& parent() { return m_parent; }
    const ParentT& parent() const { return m_parent; }
public:
    explicit SP(ParentT& t_parent) : m_parent(t_parent) {}

    [[nodiscard]] bool has_exact_solution_strategy() const { return (bool) m_exact_solution_strategy; }

    Algorithm& exact_solution_strategy() { return *m_exact_solution_strategy; }
    [[nodiscard]] const Algorithm& exact_solution_strategy() const { return *m_exact_solution_strategy; }

    template<class AlgorithmT, class ...ArgsT> AlgorithmT& set_exact_solution_strategy(ArgsT&&... t_args);

    virtual void initialize() = 0;

    virtual void solve() {
        m_exact_solution_strategy->solve();
    }

    virtual void update() = 0;

    virtual bool can_enrich_master() {
        return m_exact_solution_strategy->primal_solution().objective_value() < -ToleranceForAbsoluteGapPricing;
    }

    virtual void enrich_master_problem() = 0;
};

template<class ParentT>
template<class AlgorithmT, class... ArgsT>
AlgorithmT& SP<ParentT>::set_exact_solution_strategy(ArgsT &&... t_args) {
    auto* algorithm = new AlgorithmT(std::forward<ArgsT>(t_args)...);
    m_exact_solution_strategy.reset(algorithm);
    return *algorithm;
}

#endif //IDOL_SP_H
