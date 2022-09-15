//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_SOLUTIONSTRATEGY_H
#define OPTIMIZE_SOLUTIONSTRATEGY_H

#include "AbstractSolutionStrategy.h"
#include <memory>
#include <list>

class NodeByBound;

template<class SolverT>
class SolutionStrategy : public AbstractSolutionStrategy {
    Model& m_model;
    std::unique_ptr<SolverT> m_solver;

    std::list<std::pair<Var, double>> m_original_lower_bounds;
    std::list<std::pair<Var, double>> m_original_upper_bounds;

    SolutionStrategy(const SolutionStrategy& t_src);
public:
    explicit SolutionStrategy(Model& t_model) : m_model(t_model) {}

    SolutionStrategy(SolutionStrategy&&) noexcept = delete;

    SolutionStrategy& operator=(const SolutionStrategy&) = delete;
    SolutionStrategy& operator=(SolutionStrategy&&) noexcept = default;

    void initialize() override;

    void solve() const override { m_solver->solve(); }

    void reset_local_changes() override;

    [[nodiscard]] const Solver &solver() const override { return *m_solver; }

    void set_local_lower_bound(const Var &t_var, double t_lb) override;

    void set_local_upper_bound(const Var &t_var, double t_ub) override;
};

template<class SolverT>
SolutionStrategy<SolverT>::SolutionStrategy(const SolutionStrategy &t_src) : m_model(t_src.m_model) {

}

template<class SolverT>
void SolutionStrategy<SolverT>::initialize() {
    if (!m_solver) {
        m_solver = std::make_unique<SolverT>(m_model);
    }
}

template<class SolverT>
void SolutionStrategy<SolverT>::reset_local_changes() {
    for (auto it = m_original_lower_bounds.rbegin(), end = m_original_lower_bounds.rend() ; it != end ; ++it) {
        m_model.update_lb(it->first, it->second);
    }
    m_original_lower_bounds.clear();
    for (auto it = m_original_upper_bounds.rbegin(), end = m_original_upper_bounds.rend() ; it != end ; ++it) {
        m_model.update_ub(it->first, it->second);
    }
    m_original_upper_bounds.clear();
}

template<class SolverT>
void SolutionStrategy<SolverT>::set_local_lower_bound(const Var &t_var, double t_lb) {
    m_original_lower_bounds.template emplace_back(t_var, t_var.lb());
    m_model.update_lb(t_var, t_lb);
}

template<class SolverT>
void SolutionStrategy<SolverT>::set_local_upper_bound(const Var &t_var, double t_ub) {
    m_original_upper_bounds.template emplace_back(t_var, t_var.ub());
    m_model.update_ub(t_var, t_ub);
}

#endif //OPTIMIZE_SOLUTIONSTRATEGY_H
