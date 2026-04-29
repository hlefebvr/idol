//
// Created by Henri on 29/04/2026.
//

#ifndef IDOL_OPTIMIZERS_YASOL_H
#define IDOL_OPTIMIZERS_YASOL_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"
#include "idol/general/optimizers/Optimizer.h"

namespace idol::Optimizers::Robust {
    class Yasol;
}

class idol::Optimizers::Robust::Yasol : public Optimizer {
    const idol::Robust::Description& m_robust_description;
    const idol::Bilevel::Description* m_bilevel_description = nullptr;

    void* m_impl = nullptr;
protected:
    void build() override;
    void add(const Var& t_var) override;
    void add(const Ctr& t_ctr) override;
    void add(const QCtr& t_ctr) override;
    void remove(const Var& t_var) override;
    void remove(const Ctr& t_ctr) override;
    void remove(const QCtr& t_ctr) override;
    void update() override;
    void write(const std::string& t_name) override;
    void hook_optimize() override;
    void set_solution_index(unsigned t_index) override;
    void update_obj_sense() override;
    void update_obj() override;
    void update_rhs() override;
    void update_obj_constant() override;
    void update_mat_coeff(const Ctr& t_ctr, const Var& t_var) override;
    void update_ctr_type(const Ctr& t_ctr) override;
    void update_ctr_rhs(const Ctr& t_ctr) override;
    void update_var_type(const Var& t_var) override;
    void update_var_lb(const Var& t_var) override;
    void update_var_ub(const Var& t_var) override;
    void update_var_obj(const Var& t_var) override;
public:
    Yasol(const Model& t_parent, const idol::Robust::Description& t_robust_description, const idol::Bilevel::Description* t_bilevel_description);

    ~Yasol();

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var& t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const override;
    [[nodiscard]] double get_var_ray(const Var& t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned get_n_solutions() const override;
    [[nodiscard]] unsigned get_solution_index() const override;
};

#endif //IDOL_OPTIMIZERS_YASOL_H