//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVERS_GUROBI_H
#define OPTIMIZE_SOLVERS_GUROBI_H

#include "Solver.h"
#include "../attributes/Attributes.h"
#include "../callbacks/Callback.h"

#ifdef IDOL_USE_GUROBI
#include <gurobi_c++.h>

namespace Solvers {
    class Gurobi;
}

/**
 * Interfaces with the commerical solver Gurobi.
 */
class Solvers::Gurobi : public Solver<GRBVar, GRBConstr> {
    static GRBEnv m_env;
    GRBModel m_model;

    std::list<std::unique_ptr<::Callback>> m_callbacks;

    template<class CallbackT> class Callback;

    Attributes<AttributesSections::Base> m_attributes;

    Solution::Primal primal_solution(SolutionStatus t_status, const std::function<double()>& t_get_obj_val, const std::function<double(const GRBVar&)>& t_get_primal_value) const;
    Solution::Dual dual_solution(SolutionStatus t_status, const std::function<double()>& t_get_obj_val, const std::function<double(const GRBConstr&)>& t_get_dual_value) const;
    [[nodiscard]] SolutionStatus idol_status(int t_gurobi_status) const;
    static char gurobi_type(CtrType t_type);
    static char gurobi_type(VarType t_type);
protected:
    AbstractAttributes &attributes() override { return m_attributes; }
    [[nodiscard]] const AbstractAttributes &attributes() const override { return m_attributes; }
    void execute() override;
    void execute_iis() override;

    GRBVar create_variable_impl_with_objective_coefficient(const Var& t_var);

    GRBConstr create_constraint_impl_with_rhs(const Ctr& t_ctr);

    void set_constraint_lhs(const Ctr& t_ctr);

    void set_variable_components(const Var& t_var);

    [[nodiscard]] SolutionStatus solution_status() const;
public:
    explicit Gurobi(Model& t_model);

    void update_coefficient_rhs(const Ctr &t_ctr, double t_rhs) override;

    void remove(const Var &t_variable) override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    [[nodiscard]] Solution::Dual farkas_certificate() const override;

    [[nodiscard]] Solution::Primal unbounded_ray() const override;

    Solution::Dual iis() const override;

    void update_objective(const Row &t_objective) override;

    Var add_column(TempVar t_temporary_variable) override;

    Ctr add_row(TempCtr t_temporary_constraint) override;

    void remove(const Ctr &t_constraint) override;

    void update_lb(const Var &t_var, double t_lb) override;

    void update_ub(const Var &t_var, double t_ub) override;

    void write(const std::string &t_filename) override;

    using Solver<GRBVar, GRBConstr>::raw;

    GRBModel& raw() { return m_model; }

    [[nodiscard]] const GRBModel& raw() const { return m_model; }

    template<class T, class ...ArgsT> T& add_callback(ArgsT&& ...t_args);
};

template<class T, class... ArgsT>
T &Solvers::Gurobi::add_callback(ArgsT &&... t_args) {

    if constexpr (Callbacks::uses_lazy_cuts<T>()) {
        m_model.set(GRB_IntParam_LazyConstraints, 1);
    }

    if constexpr (Callbacks::uses_user_cuts<T>()) {
        m_model.set(GRB_IntParam_PreCrush, 1);
    }

    Callback<T>* cb;
    if constexpr (Callbacks::uses_advanced_constructor<T>()) {
        cb = new Callback<T>(*this, *this, std::forward<ArgsT>(t_args)...);
    } else {
        cb = new Callback<T>(*this, std::forward<ArgsT>(t_args)...);
    }

    m_callbacks.template emplace_back(cb);
    m_model.setCallback(cb);

    return *cb;
}

template<class CallbackT>
class Solvers::Gurobi::Callback : public CallbackT, public GRBCallback {
    Gurobi& m_solver;
    class Context;
public:
    template<class ...ArgsT> explicit Callback(Gurobi& t_solver, ArgsT&& ...t_args)
        : m_solver(t_solver), CallbackT(std::forward<ArgsT>(t_args)...) {}

    void callback() override {
        Context ctx(*this);
        this->execute(ctx);
    }

};

template<class CallbackT>
class Solvers::Gurobi::Callback<CallbackT>::Context : public ::Callback::Context {
    Callback& m_parent;
public:
    explicit Context(Callback& t_parent) : m_parent(t_parent) {}

    [[nodiscard]] Event event() const override;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    void add_lazy_cut(TempCtr t_ctr) const override;
};

template<class CallbackT>
Event Solvers::Gurobi::Callback<CallbackT>::Context::event() const {
    if (m_parent.where == GRB_CB_MIPNODE) {
        return RelaxationSolved;
    }
    if (m_parent.where == GRB_CB_MIPSOL) {
        return NewIncumbentFound;
    }
    return Unsupported;
}

template<class CallbackT>
Solution::Primal Solvers::Gurobi::Callback<CallbackT>::Context::primal_solution() const {
    if (m_parent.where == GRB_CB_MIPSOL) {
        return m_parent.m_solver.primal_solution(
                Optimal,
                [this](){ return m_parent.getDoubleInfo(GRB_CB_MIPSOL_OBJ); },
                [this](const GRBVar &t_var) { return m_parent.getSolution(t_var); }
        );
    }
    if (m_parent.where == GRB_CB_MIPNODE) {
        return m_parent.m_solver.primal_solution(
                m_parent.m_solver.idol_status(m_parent.getIntInfo(GRB_CB_MIPNODE_STATUS)),
                [this](){ return m_parent.getDoubleInfo(GRB_CB_MIPSOL_OBJ); },
                [this](const GRBVar &t_var) { return m_parent.getNodeRel(t_var); }
        );
    }
    throw Exception("Not available.");
}

template<class CallbackT>
void Solvers::Gurobi::Callback<CallbackT>::Context::add_lazy_cut(TempCtr t_ctr) const {
    GRBLinExpr expr;
    for (const auto& [var, constant] : t_ctr.row().lhs()) {
        expr += m_parent.m_solver.value(constant) * m_parent.m_solver.raw(var);
    }
    m_parent.addLazy(
            expr,
            m_parent.m_solver.gurobi_type(t_ctr.type()),
            m_parent.m_solver.value(t_ctr.row().rhs())
    );
}

#endif

#endif //OPTIMIZE_SOLVERS_GUROBI_H
