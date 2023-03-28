//
// Created by henri on 20/02/23.
//

#ifndef IDOL_MOSEK_H
#define IDOL_MOSEK_H

#ifdef IDOL_USE_MOSEK

#include "LazyBackend.h"
#include <fusion.h>

struct MosekVar {
    mosek::fusion::Variable::t variable;
    mosek::fusion::Constraint::t lower_bound;
    mosek::fusion::Constraint::t upper_bound;
};

struct MosekCtr {
    mosek::fusion::Constraint::t constraint;
};

namespace Optimizers {
    class Mosek;
}

class Optimizers::Mosek : public LazyBackend<MosekVar, MosekCtr> {
    bool m_continuous_relaxation;

    mosek::fusion::Model::t m_model;
    SolutionStatus m_solution_status = Unknown;
    SolutionReason m_solution_reason = NotSpecified;
protected:
    void set_var_attr(MosekVar& t_mosek_var, int t_type, double t_lb, double t_ub, double t_obj);
    void set_var_lb(MosekVar& t_mosek_var, double t_bound);
    void set_var_ub(MosekVar& t_mosek_var, double t_bound);
    [[nodiscard]] mosek::fusion::Expression::t to_mosek_expression(const LinExpr<Var>& t_expr) const;
    [[nodiscard]] mosek::fusion::Expression::t to_mosek_expression(const Expr<Var>& t_expr) const;

    void hook_build() override;

    void hook_optimize() override;

    void hook_write(const std::string &t_name) override;

    MosekVar hook_add(const Var &t_var, bool t_add_column) override;

    MosekCtr hook_add(const Ctr &t_ctr) override;

    void hook_update_objective_sense() override;

    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) override;

    void hook_update() override;

    void hook_update(const Var &t_var) override;

    void hook_update(const Ctr &t_ctr) override;

    void hook_update_objective() override;

    void hook_update_rhs() override;

    void hook_remove(const Var &t_var) override;

    void hook_remove(const Ctr &t_ctr) override;

    using LazyBackend::get;
    using LazyBackend::set;

    void set(const Parameter<bool>& t_param, bool t_value) override;
    void set(const Parameter<double>& t_param, double t_value) override;
    [[nodiscard]] bool get(const Parameter<bool>& t_param) const override;
    [[nodiscard]] double get(const Parameter<double>& t_param) const override;

    [[nodiscard]] int get(const Req<int, void> &t_attr) const override;
    [[nodiscard]] double get(const Req<double, void> &t_attr) const override;
    [[nodiscard]] double get(const Req<double, Var> &t_attr, const Var &t_var) const override;
    [[nodiscard]] double get(const Req<double, Ctr> &t_attr, const Ctr &t_ctr) const override;
public:
    explicit Mosek(const Model& t_model, bool t_continuous_relaxation);

    ~Mosek() override;

    [[nodiscard]] std::string name() const override { return "mosek"; }
};

#endif

#endif //IDOL_MOSEK_H
