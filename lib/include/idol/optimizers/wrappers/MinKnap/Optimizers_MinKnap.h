//
// Created by henri on 20/02/23.
//

#ifndef IDOL_OPTIMIZERS_MINKNAP_H
#define IDOL_OPTIMIZERS_MINKNAP_H

#ifdef IDOL_USE_MINKNAP

#include "idol/optimizers/wrappers/OptimizerWithLazyUpdates.h"

namespace idol::Optimizers {
    class MinKnap;
}

class idol::Optimizers::MinKnap : public OptimizerWithLazyUpdates<std::optional<double>, bool> {
    SolutionStatus m_solution_status = Loaded;
    SolutionReason m_solution_reason = NotSpecified;
    std::optional<double> m_objective_value;
protected:
    void hook_build() override;
    void hook_optimize() override;
    void hook_write(const std::string &t_name) override;
    std::optional<double> hook_add(const Var &t_var, bool t_add_column) override;
    bool hook_add(const Ctr &t_ctr) override;
    void hook_update_objective_sense() override {}
    void hook_update_matrix(const Ctr &t_ctr, const Var &t_var, const Constant &t_constant) override {}
    void hook_update() override {}
    void hook_update(const Var &t_var) override {}
    void hook_update(const Ctr &t_ctr) override {}
    void hook_update_objective() override {}
    void hook_update_rhs() override {}
    void hook_remove(const Var &t_var) override {}
    void hook_remove(const Ctr &t_ctr) override {}

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
    [[nodiscard]] unsigned int get_solution_index() const override { return 0; }
    void set_solution_index(unsigned int t_index) override;
public:
    explicit MinKnap(const Model& t_model);

    [[nodiscard]] std::string name() const override { return "minknap"; }
};

#endif

#endif //IDOL_OPTIMIZERS_MINKNAP_H
