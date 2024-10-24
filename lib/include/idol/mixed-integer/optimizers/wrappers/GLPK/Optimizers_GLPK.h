//
// Created by henri on 14/02/23.
//

#ifndef IDOL_OPTIMIZERS_GLPK_H
#define IDOL_OPTIMIZERS_GLPK_H

#include <stack>

#ifdef IDOL_USE_GLPK

#include "idol/mixed-integer/optimizers/wrappers/OptimizerWithLazyUpdates.h"
#include <glpk.h>

namespace idol::Optimizers {
    class GLPK;
}

class idol::Optimizers::GLPK  : public OptimizerWithLazyUpdates<int, int> {

    bool m_continuous_relaxation;

    glp_prob* m_model;
    glp_smcp m_simplex_parameters;
    glp_iocp m_mip_parameters;
    bool m_solved_as_mip = false;
    bool m_rebuild_basis = false;

    SolutionStatus m_solution_status = Loaded;
    SolutionReason m_solution_reason = NotSpecified;
    std::optional<PrimalPoint> m_unbounded_ray;
    std::optional<DualPoint> m_farkas_certificate;

    std::stack<int> m_deleted_variables;
    std::stack<int> m_deleted_constraints;
protected:
    void hook_build() override;

    void hook_optimize() override;

    void hook_write(const std::string &t_name) override;

    int hook_add(const Var &t_var, bool t_add_column) override;

    int hook_add(const Ctr &t_ctr) override;

    void hook_update_objective_sense() override;

    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) override;

    void hook_update() override;

    void hook_update(const Var &t_var) override;

    void hook_update(const Ctr &t_ctr) override;

    void hook_update_objective() override;

    void hook_update_rhs() override;

    void hook_remove(const Var &t_var) override;

    void hook_remove(const Ctr &t_ctr) override;

    void set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj);

    void set_ctr_attr(int t_index, int t_type, double t_rhs);

    void save_simplex_solution_status();
    void compute_farkas_certificate();
    void compute_unbounded_ray();
    void save_milp_solution_status();

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

    static Model read_from_glpk(idol::Env& t_env, glp_prob* t_model);
    static Model read_from_lp_file(Env& t_env, const std::string& t_filename);
    static Model read_from_mps_file(Env& t_env, const std::string& t_filename);
public:
    explicit GLPK(const Model& t_model, bool t_continuous_relaxation);

    [[nodiscard]] std::string name() const override { return "GLPK"; }

    void set_param_time_limit(double t_time_limit) override;

    void set_param_best_obj_stop(double t_best_obj_stop) override;

    void set_param_best_bound_stop(double t_best_bound_stop) override;

    void set_param_presolve(bool t_value) override;

    void set_param_logs(bool t_value) override;

    static Model read_from_file(Env& t_env, const std::string& t_filename);
};

#endif

#endif //IDOL_OPTIMIZERS_GLPK_H
