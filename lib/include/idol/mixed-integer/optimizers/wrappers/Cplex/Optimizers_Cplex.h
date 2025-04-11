//
// Created by henri on 07.04.25.
//

#ifndef IDOL_OPTIMIZERS_CPLEX_H
#define IDOL_OPTIMIZERS_CPLEX_H

#ifdef IDOL_USE_CPLEX
#include <ilcplex/ilocplex.h>
#include <memory>
#include <variant>

#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "CplexCallbackI.h"

namespace idol::Optimizers {
    class Cplex;

    namespace impl {
        class CplexEnvKiller;
    }
}

struct idol::Optimizers::impl::CplexEnvKiller {
    IloEnv env;
    ~CplexEnvKiller() { env.end(); }
};

class idol::Optimizers::Cplex : public OptimizerWithLazyUpdates<IloNumVar, IloRange, IloRange> {
    static std::unique_ptr<impl::CplexEnvKiller> s_global_env;

    IloEnv m_env;
    IloModel m_model;
    IloCplex m_cplex;
    IloObjective m_objective;
    bool m_continuous_relaxation;
    unsigned int m_solution_index = 0;
    bool m_lazy_cut = false;

    std::unique_ptr<CplexCallbackI> m_cplex_callback;

    IloNumVar::Type cplex_var_type(int t_type);
    static double cplex_numeric(double t_value);
protected:
    void hook_build() override;
    void hook_optimize() override;
    void hook_write(const std::string &t_name) override;
    IloNumVar hook_add(const Var& t_var, bool t_add_column) override;
    IloRange hook_add(const Ctr& t_ctr) override;
    IloRange hook_add(const QCtr& t_ctr) override;
    void hook_update(const Var& t_var) override;
    void hook_update(const Ctr& t_ctr) override;
    void hook_update_objective_sense() override;
    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) override;
    void hook_update_objective() override;
    void hook_update_rhs() override;
    void hook_update() override;
    void hook_remove(const Var& t_var) override;
    void hook_remove(const Ctr& t_ctr) override;
    void hook_remove(const QCtr& t_ctr) override;
    void update_objective_constant();

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

    void create_callback_if_not_exists();
public:
    Cplex(const Model& t_model, bool t_continuous_relaxation);

    ~Cplex() override;

    IloEnv& env() { return m_env; }

    [[nodiscard]] const IloEnv& env() const { return m_env; }

    IloModel& model() { return m_model; }

    [[nodiscard]] const IloModel& model() const { return m_model; }

    [[nodiscard]] std::string name() const override { return "Cplex"; }

    bool lazy_cuts() const { return m_lazy_cut; }

    void set_lazy_cuts(bool t_lazy_cut);

    void set_param_time_limit(double t_time_limit) override;

    void set_param_threads(unsigned int t_thread_limit) override;

    void set_param_best_obj_stop(double t_best_obj_stop) override;

    void set_param_best_bound_stop(double t_best_bound_stop) override;

    void set_param_presolve(bool t_value) override;

    void set_param_infeasible_or_unbounded_info(bool t_value) override;

    void add_callback(Callback* t_ptr_to_callback);

    void set_max_n_solution_in_pool(unsigned int t_value);

    void set_param_logs(bool t_value) override;

    void set_tol_mip_relative_gap(double t_relative_gap_tolerance) override;

    void set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) override;

    void set_tol_feasibility(double t_tol_feasibility) override;

    void set_tol_optimality(double t_tol_optimality) override;

    void set_tol_integer(double t_tol_integer) override;

    static Model read_from_file(Env& t_env, const std::string& t_filename);
};

#endif

#endif //IDOL_OPTIMIZERS_CPLEX_H
