//
// Created by henri on 31/01/23.
//

#ifndef IDOL_OPTIMIZERS_GUROBI_H
#define IDOL_OPTIMIZERS_GUROBI_H

#ifdef IDOL_USE_GUROBI
#include "gurobi_c++.h"
#include <memory>
#include <variant>

#include "idol/optimizers/wrappers/OptimizerWithLazyUpdates.h"
#include "idol/optimizers/callbacks/Callback.h"
#include "GurobiCallbackI.h"

namespace idol::Optimizers {
    class Gurobi;
}

class idol::Optimizers::Gurobi : public OptimizerWithLazyUpdates<GRBVar, std::variant<GRBConstr, GRBQConstr>> {
    friend class ::idol::GurobiCallbackI;
    static std::unique_ptr<GRBEnv> s_global_env;

    static GRBEnv& get_global_env();

    GRBEnv& m_env;
    GRBModel m_model;
    bool m_continuous_relaxation;

    std::unique_ptr<GurobiCallbackI> m_gurobi_callback;

    char gurobi_var_type(int t_type);
    static char gurobi_ctr_type(int t_type);
    static char gurobi_obj_sense(int t_sense);
    static double gurobi_numeric(double t_value);
    static VarType idol_var_type(char t_type);
    static CtrType idol_ctr_type(char t_type);
    static ObjectiveSense idol_obj_sense(int t_sense);
    [[nodiscard]] std::pair<SolutionStatus, SolutionReason> gurobi_status(int t_status) const;
protected:
    void hook_build() override;
    void hook_optimize() override;
    void hook_write(const std::string &t_name) override;
    GRBVar hook_add(const Var& t_var, bool t_add_column) override;
    std::variant<GRBConstr, GRBQConstr> hook_add(const Ctr& t_ctr) override;
    void hook_update(const Var& t_var) override;
    void hook_update(const Ctr& t_ctr) override;
    void hook_update_objective_sense() override;
    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) override;
    void hook_update_objective() override;
    void hook_update_rhs() override;
    void hook_update() override;
    void hook_remove(const Var& t_var) override;
    void hook_remove(const Ctr& t_ctr) override;

    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;

    void set_solution_index(unsigned int t_index) override;

public:
    Gurobi(const Model& t_model, bool t_continuous_relaxation, GRBEnv& t_env);
    explicit Gurobi(const Model& t_model, bool t_continuous_relaxation) : Gurobi(t_model, t_continuous_relaxation, Gurobi::get_global_env()) {}

    GRBEnv& env() { return m_env; }

    [[nodiscard]] const GRBEnv& env() const { return m_env; }

    GRBModel& model() { return m_model; }

    [[nodiscard]] const GRBModel& model() const { return m_model; }

    [[nodiscard]] std::string name() const override { return "Gurobi"; }

    void set_param_time_limit(double t_time_limit) override;

    void set_param_threads(unsigned int t_thread_limit) override;

    void set_param_best_obj_stop(double t_best_obj_stop) override;

    void set_param_best_bound_stop(double t_best_bound_stop) override;

    void set_param_presolve(bool t_value) override;

    void set_param_infeasible_or_unbounded_info(bool t_value) override;

    void add_callback(Callback* t_ptr_to_callback);

    void set_lazy_cut(bool t_value);

    void set_max_n_solution_in_pool(unsigned int t_value);

    void set_param_logs(bool t_value) override;

    void set_param(GRB_IntParam t_param, int t_value);

    void set_param(GRB_DoubleParam t_param, double t_value);

    void set_tol_mip_relative_gap(double t_relative_gap_tolerance) override;

    void set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) override;

    void set_tol_feasibility(double t_tol_feasibility) override;

    void set_tol_optimality(double t_tol_optimality) override;

    void set_tol_integer(double t_tol_integer) override;

    static Model read_from_file(Env& t_env, const std::string& t_filename);
};

#endif

#endif //IDOL_OPTIMIZERS_GUROBI_H
