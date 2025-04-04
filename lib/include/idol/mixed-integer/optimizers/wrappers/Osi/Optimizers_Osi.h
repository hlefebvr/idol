//
// Created by henri on 05.02.24.
//

#ifndef IDOL_OPTIMIZERS_OSI_H
#define IDOL_OPTIMIZERS_OSI_H

#ifdef IDOL_USE_OSI

#include <OsiSolverInterface.hpp>
#include <stack>
#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"

namespace idol::Optimizers {
    class Osi;
}

class idol::Optimizers::Osi : public OptimizerWithLazyUpdates<int, int, int> {
    bool m_continuous_relaxation;
    bool m_is_resolve = false;

    std::unique_ptr<OsiSolverInterface> m_solver_interface;
public:
    std::string name() const override;

    SolutionStatus get_status() const override;

    SolutionReason get_reason() const override;

    double get_best_obj() const override;

    double get_best_bound() const override;

    double get_var_primal(const Var &t_var) const override;

    double get_var_reduced_cost(const Var &t_var) const override;

    double get_var_ray(const Var &t_var) const override;

    double get_ctr_dual(const Ctr &t_ctr) const override;

    double get_ctr_farkas(const Ctr &t_ctr) const override;

    double get_relative_gap() const override;

    double get_absolute_gap() const override;

    unsigned int get_n_solutions() const override;

    unsigned int get_solution_index() const override;

protected:
    void hook_optimize() override;

    void set_solution_index(unsigned int t_index) override;

    void hook_build() override;

    void hook_write(const std::string &t_name) override;

    int hook_add(const Var &t_var, bool t_add_column) override;

    int hook_add(const Ctr &t_ctr) override;

    int hook_add(const QCtr &t_ctr) override;

    void hook_update_objective_sense() override;

    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, double t_constant) override;

    void hook_update() override;

    void hook_update(const Var &t_var) override;

    void hook_update(const Ctr &t_ctr) override;

    void hook_update_objective() override;

    void hook_update_rhs() override;

    void hook_remove(const Var &t_var) override;

    void hook_remove(const Ctr &t_ctr) override;

    void hook_remove(const QCtr &t_ctr) override;
public:
    Osi(const Model& t_model, const OsiSolverInterface& t_solver_interface, bool t_continuous_relaxation);

    void set_param_logs(bool t_value) override;

    void set_param_presolve(bool t_value) override;
};

#endif

#endif //IDOL_OPTIMIZERS_OSI_H
