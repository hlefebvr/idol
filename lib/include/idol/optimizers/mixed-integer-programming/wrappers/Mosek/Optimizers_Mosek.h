//
// Created by henri on 20/02/23.
//

#ifndef IDOL_OPTIMIZERS_MOSEK_H
#define IDOL_OPTIMIZERS_MOSEK_H

#ifdef IDOL_USE_MOSEK

#include "idol/optimizers/mixed-integer-programming/wrappers/OptimizerWithLazyUpdates.h"
#include "MosekCallbackI.h"
#include <fusion.h>

namespace idol {

    struct MosekKiller {
        ~MosekKiller();
    };

    static const MosekKiller s_mosek_killer;

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
}

class idol::Optimizers::Mosek : public OptimizerWithLazyUpdates<MosekVar, MosekCtr> {
    bool m_continuous_relaxation;

    mosek::fusion::Model::t m_model;
    SolutionStatus m_solution_status = Loaded;
    SolutionReason m_solution_reason = NotSpecified;

    std::unique_ptr<MosekCallbackI> m_mosek_callback;
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

    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;
    void set_solution_index(unsigned int t_index) override;
public:
    explicit Mosek(const Model& t_model, bool t_continuous_relaxation);

    ~Mosek() override;

    [[nodiscard]] std::string name() const override { return "Mosek"; }

    void set_param_time_limit(double t_time_limit) override;

    void set_param_threads(unsigned int t_thread_limit) override;

    void set_param_best_obj_stop(double t_best_obj_stop) override;

    void set_param_best_bound_stop(double t_best_bound_stop) override;

    void set_param_presolve(bool t_value) override;

    void set_param_infeasible_or_unbounded_info(bool t_value) override;

    void set_parameter(const std::string& t_param, double t_value);

    void set_parameter(const std::string& t_param, int t_value);

    void set_parameter(const std::string& t_param, const std::string& t_value);

    void set_param_logs(bool t_value) override;

    void add_callback(Callback* t_ptr_to_callback);
};

#endif

#endif //IDOL_OPTIMIZERS_MOSEK_H
