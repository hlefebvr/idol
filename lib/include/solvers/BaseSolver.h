//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_BASESOLVER_H
#define OPTIMIZE_BASESOLVER_H

#include <vector>
#include "modeling/Types.h"
#include "../modeling/models/Model.h"
#include "Solver.h"

template<class VarT, class CtrT>
class BaseSolver : public Listener, public Solver {
    const Model& m_src_model;
    std::vector<VarT> m_variables;
    std::vector<CtrT> m_constraints;
protected:
    explicit BaseSolver(const Model& t_model);

    void on_add(const Var &t_var) final;
    void on_add(const Ctr &t_ctr) final;
    void on_remove(const Var& t_var) final;
    void on_remove(const Ctr& t_ctr) final;
    void on_update_objective(const Var &t_var, const Coefficient &t_coeff) final;
    void on_update_objective_offset(const Coefficient &t_offset) final;
    void on_update_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) final;
    void on_update_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) final;
    void on_update_lb(const Var &t_var, double t_lb) final;
    void on_update_ub(const Var &t_var, double t_ub) final;
    void on_update_type(const Var &t_var, VarType t_type) final;
    void on_update_type(const Ctr &t_ctr, CtrType t_type) final;

    [[nodiscard]] double value(const Coefficient& t_coefficient) const;

    void init_model(const Model& t_model);
    virtual VarT create_variable(const Var& t_var) = 0;
    virtual CtrT create_constraint(const Ctr& t_ctr) = 0;
    virtual void fill_column(const Var& t_var) = 0;
    virtual void fill_row(const Ctr& t_ctr) = 0;
    virtual void remove_variable(const Var& t_var) = 0;
    virtual void remove_constraint(const Ctr& t_ctr) = 0;
    virtual void set_objective_coefficient(const Var &t_var, const Coefficient &t_coeff) = 0;
    virtual void set_objective_offset(const Coefficient& t_offset) = 0;
    virtual void set_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) = 0;
    virtual void set_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) = 0;
    virtual void set_lb(const Var &t_var, double t_lb) = 0;
    virtual void set_ub(const Var &t_var, double t_ub) = 0;
    virtual void set_type(const Var &t_var, VarType t_type) = 0;
    virtual void set_type(const Ctr &t_ctr, CtrType t_type) = 0;
    [[nodiscard]] virtual SolutionStatus get_primal_status() const = 0;
    [[nodiscard]] virtual SolutionStatus get_dual_status() const;
    [[nodiscard]] virtual double get_primal_objective_value() const = 0;
    [[nodiscard]] virtual double get_dual_objective_value() const;
    [[nodiscard]] virtual double get_primal_value(const Var& t_var) const = 0;
    [[nodiscard]] virtual double get_unbounded_ray(const Var& t_var) const = 0;
    [[nodiscard]] virtual double get_unbounded_ray_objective_value() const = 0;
    [[nodiscard]] virtual double get_dual_value(const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual double get_dual_farkas_objective_value() const = 0;
    [[nodiscard]] virtual double get_dual_farkas_value(const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual bool get_iis(const Ctr& t_ctr) const;

    [[nodiscard]] const Model& source_model() const { return m_src_model; }
public:

    VarT& get(const Var& t_var);

    const VarT& get(const Var& t_var) const;

    CtrT& get(const Ctr& t_ctr);

    const CtrT& get(const Ctr& t_ctr) const;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Primal unbounded_ray() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    [[nodiscard]] Solution::Dual dual_farkas() const override;

    [[nodiscard]] Solution::Dual iis() const override;
};

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_add(const Var &t_var) {
    m_variables.template emplace_back(create_variable(t_var));
    fill_column(t_var);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_add(const Ctr &t_ctr) {
    m_constraints.template emplace_back(create_constraint(t_ctr));
    fill_row(t_ctr);
}

template<class VarT, class CtrT>
VarT &BaseSolver<VarT, CtrT>::get(const Var &t_var) {
    return m_variables[t_var.index()];
}

template<class VarT, class CtrT>
const VarT &BaseSolver<VarT, CtrT>::get(const Var &t_var) const {
    return m_variables[t_var.index()];
}

template<class VarT, class CtrT>
CtrT &BaseSolver<VarT, CtrT>::get(const Ctr &t_ctr) {
    return m_constraints[t_ctr.index()];
}

template<class VarT, class CtrT>
const CtrT &BaseSolver<VarT, CtrT>::get(const Ctr &t_ctr) const {
    return m_constraints[t_ctr.index()];
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_remove(const Var &t_var) {
    remove_variable(t_var);

    m_variables[t_var.index()] = std::move(m_variables.back());
    m_variables.pop_back();
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_remove(const Ctr &t_ctr) {
    remove_constraint(t_ctr);

    m_constraints[t_ctr.index()] = std::move(m_constraints.back());
    m_constraints.pop_back();
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_objective(const Var &t_var, const Coefficient &t_coeff) {
    set_objective_coefficient(t_var, t_coeff);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_rhs(const Ctr &t_ctr, const Coefficient &t_coeff) {
    set_rhs(t_ctr, t_coeff);
}

template<class VarT, class CtrT>
void
BaseSolver<VarT, CtrT>::on_update_coefficient(const Ctr &t_ctr, const Var &t_var, const Coefficient &t_coefficient) {
    set_coefficient(t_ctr, t_var, t_coefficient);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_lb(const Var &t_var, double t_lb) {
    set_lb(t_var, t_lb);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_ub(const Var &t_var, double t_ub) {
    set_ub(t_var, t_ub);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_type(const Var &t_var, VarType t_type) {
    set_type(t_var, t_type);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_type(const Ctr &t_ctr, CtrType t_type) {
    set_type(t_ctr, t_type);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::on_update_objective_offset(const Coefficient &t_offset) {
    set_objective_offset(t_offset);
}

template<class VarT, class CtrT>
BaseSolver<VarT, CtrT>::BaseSolver(const Model &t_model) : m_src_model(t_model) {
    t_model.add_listener(*this);
}

template<class VarT, class CtrT>
void BaseSolver<VarT, CtrT>::init_model(const Model &t_model) {
    for (const auto& var : t_model.variables()) {
        m_variables.template emplace_back(create_variable(var));
    }

    for (const auto& ctr : t_model.constraints()) {
        m_constraints.template emplace_back(create_constraint(ctr));
        fill_row(ctr);
    }

}

template<class VarT, class CtrT>
double BaseSolver<VarT, CtrT>::value(const Coefficient &t_coefficient) const {
    return t_coefficient.constant();
}

template<class VarT, class CtrT>
Solution::Primal BaseSolver<VarT, CtrT>::primal_solution() const {
    Solution::Primal result;
    const auto status = get_primal_status();
    result.set_status(status);

    if (status == Unbounded) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (is_in(status, { Infeasible, InfeasibleTimeLimit })) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        result.set_objective_value(0.);
        return result;
    }

    result.set_objective_value(get_primal_objective_value());

    for (const auto& var : m_src_model.variables()) {
        result.set(var, get_primal_value(var));
    }

    return result;
}

template<class VarT, class CtrT>
Solution::Primal BaseSolver<VarT, CtrT>::unbounded_ray() const {

    if (get_primal_status() != Unbounded) {
        throw std::runtime_error("Only available for unbounded problems.");
    }

    if (!infeasible_or_unbounded_info()) {
        throw std::runtime_error("Turn on infeasible_or_unbounded_info before solving your model to access extreme ray information.");
    }

    Solution::Primal result;
    result.set_status(algorithm_for_lp() == PrimalSimplex ? Optimal : Feasible);
    result.set_objective_value(get_unbounded_ray_objective_value());

    for (const auto& var : m_src_model.variables()) {
        result.set(var, get_unbounded_ray(var));
    }

    return result;
}

template<class VarT, class CtrT>
Solution::Dual BaseSolver<VarT, CtrT>::dual_solution() const {
    Solution::Dual result;
    const auto dual_status = get_dual_status();
    result.set_status(dual_status);

    if (dual_status == Unbounded) {
        result.set_objective_value(+Inf);
        return result;
    }

    if (is_in(dual_status, { Infeasible, InfeasibleTimeLimit })) {
        result.set_objective_value(-Inf);
        return result;
    }

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        result.set_objective_value(0.);
        return result;
    }

    result.set_objective_value(get_dual_objective_value());

    if (!is_in(result.status(), { Optimal, Feasible, FeasibleTimeLimit })) {
        return result;
    }

    for (const auto& ctr : m_src_model.constraints()) {
        result.set(ctr, get_dual_value(ctr));
    }

    return result;
}

template<class VarT, class CtrT>
SolutionStatus BaseSolver<VarT, CtrT>::get_dual_status() const {
    return dual(get_primal_status());
}

template<class VarT, class CtrT>
double BaseSolver<VarT, CtrT>::get_dual_objective_value() const {
    return get_primal_objective_value();
}

template<class VarT, class CtrT>
Solution::Dual BaseSolver<VarT, CtrT>::dual_farkas() const {

    if (get_primal_status() != Infeasible) {
        throw std::runtime_error("Only available for infeasible problems.");
    }

    if (!infeasible_or_unbounded_info()) {
        throw std::runtime_error("Turn on infeasible_or_unbounded_info before solving your model to access farkas dual information.");
    }

    Solution::Dual result;
    result.set_status(algorithm_for_lp() == DualSimplex ? Optimal : Feasible);

    result.set_objective_value(get_dual_farkas_objective_value());

    for (const auto& ctr : m_src_model.constraints()) {
        result.set(ctr, get_dual_farkas_value(ctr));
    }

    return result;
}

template<class VarT, class CtrT>
bool BaseSolver<VarT, CtrT>::get_iis(const Ctr &t_ctr) const {
    throw std::runtime_error("Not available.");
}

template<class VarT, class CtrT>
Solution::Dual BaseSolver<VarT, CtrT>::iis() const {

    if (get_primal_status() != Infeasible) {
        throw std::runtime_error("Only available for infeasible problems.");
    }

    if (!infeasible_or_unbounded_info()) {
        throw std::runtime_error("Turn on infeasible_or_unbounded_info before solving your model to access farkas dual information.");
    }

    Solution::Dual result;

    result.set_status(Infeasible);
    result.set_objective_value(Inf);

    for (const auto& ctr : m_src_model.constraints()) {
        result.set(ctr, get_iis(ctr));
    }

    return result;
}

#endif //OPTIMIZE_BASESOLVER_H
