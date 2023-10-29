//
// Created by henri on 14/02/23.
//

#ifndef IDOL_OPTIMIZERS_HIGHS_H
#define IDOL_OPTIMIZERS_HIGHS_H

#ifdef IDOL_USE_HIGHS

#include "idol/optimizers/wrappers/OptimizerWithLazyUpdates.h"
#include <Highs.h>
#include <stack>

namespace idol::Optimizers {
    class HiGHS;
}

class idol::Optimizers::HiGHS  : public OptimizerWithLazyUpdates<int, int> {

    bool m_continuous_relaxation;

    ::Highs m_model;

    SolutionStatus m_solution_status = Loaded;
    SolutionReason m_solution_reason = NotSpecified;
    double* m_extreme_ray = nullptr;
    double* m_farkas_certificate = nullptr;
protected:
    void hook_build() override;

    void hook_optimize() override;

    void run_without_presolve();

    void hook_write(const std::string &t_name) override;

    int hook_add(const Var &t_var, bool t_add_column) override;

    int hook_add(const Ctr &t_ctr) override;

    void hook_update_objective_sense() override;

    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) override;

    void hook_update() override;

    void hook_update(const Var &t_var) override;

    void hook_update(const Ctr &t_ctr) override;

    void hook_update_objective() override;

    void hook_update_rhs() override;

    void hook_remove(const Var &t_var) override;

    void hook_remove(const Ctr &t_ctr) override;

    void set_var_attr(int t_index, int t_type, double t_lb, double t_ub, double t_obj);

    void set_var_type(int t_index, int t_type);

    void set_ctr_attr(int t_index, int t_type, double t_rhs);

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

    void analyze_status();
public:
    explicit HiGHS(const Model& t_model, bool t_continuous_relaxation);

    [[nodiscard]] std::string name() const override { return "glpk"; }

    void set_param_time_limit(double t_time_limit) override;

    void set_param_best_obj_stop(double t_best_obj_stop) override;

    void set_param_best_bound_stop(double t_best_bound_stop) override;

    void set_param_presolve(bool t_value) override;

    void set_param_log_level(LogLevel t_log_level) override;

};

#endif

#endif //IDOL_OPTIMIZERS_HIGHS_H
