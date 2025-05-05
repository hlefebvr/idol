//
// Created by henri on 29.11.24.
//

#ifndef IDOL_OPTIMIZERS_MINMAX_DUALIZE_H
#define IDOL_OPTIMIZERS_MINMAX_DUALIZE_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/general/optimizers/Algorithm.h"
#include "idol/mixed-integer/modeling/models/KKT.h"

namespace idol::Optimizers::Bilevel::MinMax {
    class Dualize;
}

class idol::Optimizers::Bilevel::MinMax::Dualize : public Algorithm {
    const idol::Bilevel::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer;
    std::unique_ptr<Model> m_deterministic_model;
    std::unique_ptr<Model> m_lower_level_model;
    std::unique_ptr<Reformulators::KKT::BoundProvider> m_bound_provider;

    const bool m_skip_solving_lower_level = false;

    void throw_if_no_deterministic_model() const;
    void solve_lower_level();

    void create_deterministic_model_if_not_exists();
public:
    Dualize(const Model& t_parent,
            const idol::Bilevel::Description& t_description,
            const OptimizerFactory& t_deterministic_optimizer,
            const std::unique_ptr<Reformulators::KKT::BoundProvider>& t_bound_provider);

    [[nodiscard]] std::string name() const override;

    [[nodiscard]] SolutionStatus get_status() const override;

    [[nodiscard]] SolutionReason get_reason() const override;

    [[nodiscard]] double get_best_obj() const override;

    [[nodiscard]] double get_best_bound() const override;

    [[nodiscard]] double get_var_primal(const Var &t_var) const override;

    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;

    [[nodiscard]] double get_var_ray(const Var &t_var) const override;

    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;

    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;

    [[nodiscard]] unsigned int get_n_solutions() const override;

    [[nodiscard]] unsigned int get_solution_index() const override;

protected:
    void add(const Var &t_var) override;

    void add(const Ctr &t_ctr) override;

    void add(const QCtr &t_ctr) override;

    void remove(const Var &t_var) override;

    void remove(const Ctr &t_ctr) override;

    void remove(const QCtr &t_ctr) override;

    void update() override;

    void write(const std::string &t_name) override;

    void hook_optimize() override;

    void set_solution_index(unsigned int t_index) override;

    void update_obj_sense() override;

    void update_obj() override;

    void update_rhs() override;

    void update_obj_constant() override;

    void update_mat_coeff(const Ctr &t_ctr, const Var &t_var) override;

    void update_ctr_type(const Ctr &t_ctr) override;

    void update_ctr_rhs(const Ctr &t_ctr) override;

    void update_var_type(const Var &t_var) override;

    void update_var_lb(const Var &t_var) override;

    void update_var_ub(const Var &t_var) override;

    void update_var_obj(const Var &t_var) override;
};

#endif //IDOL_OPTIMIZERS_MINMAX_DUALIZE_H
