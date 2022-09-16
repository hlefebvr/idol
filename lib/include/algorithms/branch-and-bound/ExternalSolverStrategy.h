//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_EXTERNALSOLVERSTRATEGY_H
#define OPTIMIZE_EXTERNALSOLVERSTRATEGY_H

#include "AbstractSolutionStrategy.h"
#include <memory>
#include <list>

class NodeByBound;
class Model;

template<class SolverT>
class ExternalSolverStrategy : public AbstractSolutionStrategy {
protected:
    Model& m_model;
    std::unique_ptr<SolverT> m_solver;

    std::list<std::pair<Var, double>> m_original_lower_bounds;
    std::list<std::pair<Var, double>> m_original_upper_bounds;

    ExternalSolverStrategy(const ExternalSolverStrategy& t_src);
public:
    explicit ExternalSolverStrategy(Model& t_model) : m_model(t_model) {}

    ExternalSolverStrategy(ExternalSolverStrategy&&) noexcept = delete;

    ExternalSolverStrategy& operator=(const ExternalSolverStrategy&) = delete;
    ExternalSolverStrategy& operator=(ExternalSolverStrategy&&) noexcept = default;

    void build() override;

    void solve() override { m_solver->solve(); }

    Model& model() { return m_model; }

    [[nodiscard]] const Model& model() const { return m_model; }

    void reset_local_changes() override;

    [[nodiscard]] Solution::Primal primal_solution() const override { return m_solver->primal_solution(); }

    [[nodiscard]] Solution::Dual dual_solution() const override { return m_solver->dual_solution(); }

    [[nodiscard]] Solution::Dual farkas_certificate() const override { return m_solver->dual_farkas(); }

    void set_local_lower_bound(const Var &t_var, double t_lb) override;

    void set_local_upper_bound(const Var &t_var, double t_ub) override;

    void update_objective(const Row &t_objective) override;

    void add_column(TempVar t_temporary_variable) override;
};

template<class SolverT>
ExternalSolverStrategy<SolverT>::ExternalSolverStrategy(const ExternalSolverStrategy &t_src) : m_model(t_src.m_model) {

}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::build() {
    if (!m_solver) {
        m_solver = std::make_unique<SolverT>(m_model);
    }
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::reset_local_changes() {
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
void ExternalSolverStrategy<SolverT>::set_local_lower_bound(const Var &t_var, double t_lb) {
    m_original_lower_bounds.template emplace_back(t_var, t_var.lb());
    m_model.update_lb(t_var, t_lb);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::set_local_upper_bound(const Var &t_var, double t_ub) {
    m_original_upper_bounds.template emplace_back(t_var, t_var.ub());
    m_model.update_ub(t_var, t_ub);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::update_objective(const Row &t_objective) {
    m_model.update_objective(t_objective);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::add_column(TempVar t_temporary_variable) {
    m_model.add_variable(std::move(t_temporary_variable));
}

#endif //OPTIMIZE_EXTERNALSOLVERSTRATEGY_H
