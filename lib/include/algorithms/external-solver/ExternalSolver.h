//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_EXTERNALSOLVER_H
#define OPTIMIZE_EXTERNALSOLVER_H

#include "algorithms/Algorithm.h"
#include "modeling/models/Model.h"
#include "algorithms/attributes/Attributes_Base.h"
#include "algorithms/attributes/Attributes.h"
#include <memory>
#include <list>

template<class SolverT>
class ExternalSolver : public Algorithm {
protected:
    Model& m_model;
    SolverT m_solver;
    Attributes<AttributesSections::Base> m_attributes;

    AbstractAttributes &attributes() override { return m_attributes; }

    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
public:
    explicit ExternalSolver(Model& t_model) : m_model(t_model), m_solver(t_model) {

        m_attributes.template set_callback<Attr::InfeasibleOrUnboundedInfo>([this](bool t_value) {
            m_solver.set_infeasible_or_unbounded_info(t_value);
        });

        m_attributes.template set_callback<Attr::Presolve>([this](bool t_value) {
           m_solver.set_presolve(t_value);
        });

    }

    ExternalSolver(const ExternalSolver& t_src) = delete;
    ExternalSolver(ExternalSolver&&) noexcept = delete;

    ExternalSolver& operator=(const ExternalSolver&) = delete;
    ExternalSolver& operator=(ExternalSolver&&) noexcept = default;

    void build() override;

    void execute() override { m_solver.solve(); }

    [[nodiscard]] const Model& model() const { return m_model; }

    [[nodiscard]] Solution::Primal primal_solution() const override { return m_solver.primal_solution(); }

    [[nodiscard]] Solution::Dual dual_solution() const override { return m_solver.dual_solution(); }

    [[nodiscard]] Solution::Dual farkas_certificate() const override { return m_solver.dual_farkas(); }

    void set_lower_bound(const Var &t_var, double t_lb) override;

    void set_upper_bound(const Var &t_var, double t_ub) override;

    void set_objective(const Row &t_objective) override;

    Var add_column(TempVar t_temporary_variable) override;

    void remove_variable(const Var &t_variable) override;

    Ctr add_constraint(TempCtr t_temporary_constraint) override;

    void update_constraint_rhs(const Ctr &t_ctr, double t_rhs) override;

    void remove_constraint(const Ctr &t_constraint) override;

    void compute_iis() override;

    [[nodiscard]] Solution::Dual iis() const override;
};

template<class SolverT>
void ExternalSolver<SolverT>::build() {}

template<class SolverT>
void ExternalSolver<SolverT>::set_lower_bound(const Var &t_var, double t_lb) {
    m_model.update_lb(t_var, t_lb);
}

template<class SolverT>
void ExternalSolver<SolverT>::set_upper_bound(const Var &t_var, double t_ub) {
    m_model.update_ub(t_var, t_ub);
}

template<class SolverT>
void ExternalSolver<SolverT>::set_objective(const Row &t_objective) {
    m_model.update_objective(t_objective);
}

template<class SolverT>
Var ExternalSolver<SolverT>::add_column(TempVar t_temporary_variable) {
    return m_model.add_variable(std::move(t_temporary_variable));
}

template<class SolverT>
Ctr ExternalSolver<SolverT>::add_constraint(TempCtr t_temporary_constraint) {
    return { m_model.add_constraint(std::move(t_temporary_constraint)) };
}

template<class SolverT>
void ExternalSolver<SolverT>::update_constraint_rhs(const Ctr &t_ctr, double t_rhs) {
    m_model.update_rhs(t_ctr, t_rhs);
}

template<class SolverT>
void ExternalSolver<SolverT>::remove_variable(const Var &t_variable) {
    m_model.remove(t_variable);
}

template<class SolverT>
void ExternalSolver<SolverT>::remove_constraint(const Ctr &t_constraint) {
    m_model.remove(t_constraint);
}

template<class SolverT>
Solution::Dual ExternalSolver<SolverT>::iis() const {
    return m_solver.iis();
}

template<class SolverT>
void ExternalSolver<SolverT>::compute_iis() {
    m_solver.compute_iis();
}

#endif //OPTIMIZE_EXTERNALSOLVER_H
