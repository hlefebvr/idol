//
// Created by Henri on 26/05/2026.
//

#ifndef IDOL_OPTIMIZERS_CONVEXIFICATION_H
#define IDOL_OPTIMIZERS_CONVEXIFICATION_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/general/optimizers/Algorithm.h"

namespace idol::Robust {
    class Description;
}

namespace idol::Optimizers::Robust {
    class Convexification;
}

class idol::Optimizers::Robust::Convexification : public Algorithm {
    const idol::Robust::Description& m_robust_description;
    const idol::Bilevel::Description& m_bilevel_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_integer_master_heuristic_optimizer;
    std::unique_ptr<OptimizerFactory> m_sub_problem_optimizer;

    // Model
    std::unique_ptr<Model> m_deterministic_model;
    std::list<idol::Var> m_first_stage_variables;
    std::optional<Annotation<unsigned int>> m_decomposition;

    void throw_if_no_deterministic_model() const;
    void build_deterministic_model();
    void build_branch_and_price();
public:
    Convexification(const Model& t_parent,
        const idol::Robust::Description& t_robust_description,
        const idol::Bilevel::Description& t_bilevel_description,
        const OptimizerFactory& t_master_optimizer,
        const OptimizerFactory& t_sub_problem_optimizer,
        const OptimizerFactory* t_integer_master_heuristic_optimizer);

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

    [[nodiscard]] const Model& deterministic_model() const;

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

#endif //IDOL_OPTIMIZERS_CONVEXIFICATION_H