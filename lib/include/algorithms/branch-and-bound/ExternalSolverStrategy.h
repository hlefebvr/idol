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

    [[nodiscard]] Solution::Primal primal_solution() const override { return m_solver->primal_solution(); }

    [[nodiscard]] Solution::Dual dual_solution() const override { return m_solver->dual_solution(); }

    [[nodiscard]] Solution::Dual farkas_certificate() const override { return m_solver->dual_farkas(); }

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;

    void set_objective(const Row &t_objective) override;

    Var add_column(TempVar t_temporary_variable) override;

    void remove_variable(const Var &t_variable) override;

    Ctr add_constraint(TempCtr t_temporary_constraint) override;

    void update_constraint_rhs(const Ctr &t_ctr, double t_rhs) override;

    void remove_constraint(const Ctr &t_constraint) override;
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
void ExternalSolverStrategy<SolverT>::set_lower_bound(const Var &t_var, double t_lb) {
    m_original_lower_bounds.template emplace_back(t_var, t_var.lb());
    m_model.update_lb(t_var, t_lb);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::set_upper_bound(const Var &t_var, double t_ub) {
    m_original_upper_bounds.template emplace_back(t_var, t_var.ub());
    m_model.update_ub(t_var, t_ub);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::set_objective(const Row &t_objective) {
    m_model.update_objective(t_objective);
}

template<class SolverT>
Var ExternalSolverStrategy<SolverT>::add_column(TempVar t_temporary_variable) {
    return m_model.add_variable(std::move(t_temporary_variable));
}

template<class SolverT>
Ctr ExternalSolverStrategy<SolverT>::add_constraint(TempCtr t_temporary_constraint) {
    return m_model.add_constraint(std::move(t_temporary_constraint));
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {
    m_model.update_rhs(t_ctr, t_rhs);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::remove_variable(const Var &t_variable) {
    m_model.remove(t_variable);
}

template<class SolverT>
void ExternalSolverStrategy<SolverT>::remove_constraint(const Ctr &t_constraint) {
    m_model.remove(t_constraint);
}

#endif //OPTIMIZE_EXTERNALSOLVERSTRATEGY_H
