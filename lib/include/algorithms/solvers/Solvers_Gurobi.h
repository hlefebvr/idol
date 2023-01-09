//
// Created by henri on 11/10/22.
//

#ifndef OPTIMIZE_SOLVERS_GUROBI_H
#define OPTIMIZE_SOLVERS_GUROBI_H

#include "Solver.h"
#include "../callbacks/Callback.h"
#include <utility>

#ifdef IDOL_USE_GUROBI
#include <gurobi_c++.h>

namespace Solvers {
    class Gurobi;
}

/**
 * Interfaces with the commercial solver Gurobi.
 */
class Solvers::Gurobi : public Solver<GRBVar, std::variant<GRBConstr, GRBQConstr>> {
    static GRBEnv m_env;
    GRBModel m_model;

    class CallbackProxy;

    bool m_is_in_callback = false;
    std::unique_ptr<CallbackProxy> m_callback;

    void analyze_status();
    Solution::Primal primal_solution(SolutionStatus t_status, Reason t_reason, const std::function<double()>& t_get_obj_val, const std::function<double(const GRBVar&)>& t_get_primal_value) const;
    Solution::Dual dual_solution(SolutionStatus t_status, const std::function<double()>& t_get_obj_val, const std::function<double(const std::variant<GRBConstr, GRBQConstr>&)>& t_get_dual_value) const;
    static char gurobi_var_type(int t_type);
    static char gurobi_ctr_type(int t_type);
protected:
    void execute() override;
    void execute_iis() override;

    GRBVar create(const Var& t_var, bool t_with_collaterals) override;
    std::variant<GRBConstr, GRBQConstr> create(const Ctr& t_ctr, bool t_with_collaterals) override;

    void update(const Var& t_var, GRBVar& t_impl) override;
    void update(const Ctr& t_ctr, std::variant<GRBConstr, GRBQConstr>& t_impl) override;

    void update_obj() override;

    void remove(const Var &t_var, GRBVar &t_impl) override;
    void remove(const Ctr &t_ctr, std::variant<GRBConstr, GRBQConstr> &t_impl) override;
public:
    explicit Gurobi(Model& t_model);

    using Solver<GRBVar, std::variant<GRBConstr, GRBQConstr>>::update;
    using Solver<GRBVar, std::variant<GRBConstr, GRBQConstr>>::remove;

    [[nodiscard]] Solution::Primal primal_solution() const override;

    [[nodiscard]] Solution::Dual dual_solution() const override;

    [[nodiscard]] Solution::Dual farkas_certificate() const override;

    [[nodiscard]] Solution::Primal unbounded_ray() const override;

    [[nodiscard]] Solution::Dual iis() const override;

    void write(const std::string &t_filename) override;

    GRBModel& impl() { return m_model; }

    [[nodiscard]] const GRBModel& impl() const { return m_model; }

    Ctr add_ctr(TempCtr &&t_temporary_constraint) override;

    using Solver::set;
    using Solver::get;

    void set(const Parameter<double> &t_param, double t_value) override;
    void set(const Parameter<bool> &t_param, bool t_value) override;
    void set(const Parameter<int> &t_param, int t_value) override;

    void set(const AttributeWithTypeAndArguments<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override;
    void set(const AttributeWithTypeAndArguments<double, Var>& t_attr, const Var& t_var, double t_value) override;
    void set(const AttributeWithTypeAndArguments<int, Var>& t_attr, const Var& t_var, int t_value) override;

    [[nodiscard]] double get(const AttributeWithTypeAndArguments<double, void>& t_attr) const override;

};

class Solvers::Gurobi::CallbackProxy : public GRBCallback {
    friend class Gurobi;
    Gurobi& m_parent;
protected:
    void callback() override;
public:
    explicit CallbackProxy(Gurobi& t_parent) : m_parent(t_parent) {

    }
};

/*

template<class CallbackT>
Ctr Solvers::Gurobi::Callback<CallbackT>::Context::add_lazy_cut(TempCtr t_ctr) {
    GRBLinExpr expr;
    for (const auto& [var, constant] : t_ctr.row().linear()) {
        expr += m_parent.m_solver.value(constant) * m_parent.m_solver.future(var).impl();
    }
    m_parent.addLazy(
            expr,
            m_parent.m_solver.gurobi_ctr_type(t_ctr.type()),
            m_parent.m_solver.value(t_ctr.row().rhs())
    );
    auto result = m_parent.m_solver.model().add_ctr(std::move(t_ctr));
    m_parent.m_solver.add_future(result);
    return result;
}
*/
#endif

#endif //OPTIMIZE_SOLVERS_GUROBI_H
