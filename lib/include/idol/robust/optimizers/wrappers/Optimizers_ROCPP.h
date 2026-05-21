//
// Created by Henri on 13/05/2026.
//

#ifndef IDOL_OPTIMIZERS_ROCPP_KADAPTABILITY_H
#define IDOL_OPTIMIZERS_ROCPP_KADAPTABILITY_H

#include "idol/general/optimizers/Optimizer.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"
#include "idol/robust/optimizers/wrappers/ROCPP.h"

namespace idol::Optimizers::Robust {
    class ROCPP;
}

#define THROW_NOT_IMPLEMENTED { throw std::runtime_error("Not implemented " + std::string(__FUNCTION__) + ".\n" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "\n"); }

class idol::Optimizers::Robust::ROCPP : public Optimizer {
    const idol::Bilevel::Description& m_bilevel_description;
    const idol::Robust::Description& m_robust_description;
    idol::Robust::ROCPP::Approximation m_approximation;
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    std::unique_ptr<Model> m_model;
protected:
    void build() override {}
    void add(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void add(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void add(const QCtr& t_ctr) override THROW_NOT_IMPLEMENTED
    void remove(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void remove(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void remove(const QCtr& t_ctr) override THROW_NOT_IMPLEMENTED
    void update() override {}
    void write(const std::string& t_name) override THROW_NOT_IMPLEMENTED
    void hook_optimize() override;
    void set_solution_index(unsigned t_index) override THROW_NOT_IMPLEMENTED
    void update_obj_sense() override THROW_NOT_IMPLEMENTED
    void update_obj() override THROW_NOT_IMPLEMENTED
    void update_rhs() override THROW_NOT_IMPLEMENTED
    void update_obj_constant() override THROW_NOT_IMPLEMENTED
    void update_mat_coeff(const Ctr& t_ctr, const Var& t_var) override THROW_NOT_IMPLEMENTED
    void update_ctr_type(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void update_ctr_rhs(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void update_var_type(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void update_var_lb(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void update_var_ub(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void update_var_obj(const Var& t_var) override THROW_NOT_IMPLEMENTED

    void throw_if_no_deterministic_model() const;
public:
    ROCPP(const Model& t_parent,
          const idol::Robust::Description& t_robust_description,
          const idol::Bilevel::Description& t_bilevel_description,
          idol::Robust::ROCPP::Approximation t_approximation,
          const OptimizerFactory& t_optimizer);

    [[nodiscard]] std::string name() const override { return "ROC++"; }
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

    static bool is_available();
};

#endif //IDOL_OPTIMIZERS_ROCPP_KADAPTABILITY_H