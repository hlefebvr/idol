//
// Created by henri on 20.02.25.
//

#ifndef IDOL_OPTIMIZERS_PESSIMISTICASOPTIMISTIC_H
#define IDOL_OPTIMIZERS_PESSIMISTICASOPTIMISTIC_H

#include "idol/general/optimizers/Algorithm.h"
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Optimizers::Bilevel {
    class PessimisticAsOptimistic;
}

class idol::Optimizers::Bilevel::PessimisticAsOptimistic : public Algorithm {
    const ::idol::Bilevel::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_optimistic_bilevel_optimizer;

    std::optional<Model> m_optimistic_bilevel_model;
    std::optional<::idol::Bilevel::Description> m_optimistic_bilevel_annotation;
    std::optional<PrimalPoint> m_follower_solution;
public:
    PessimisticAsOptimistic(const Model& t_parent,
                            const ::idol::Bilevel::Description& t_description,
                            const OptimizerFactory& t_optimistic_bilevel_optimizer);

    std::string name() const override;

    SolutionStatus get_status() const override;

    SolutionReason get_reason() const override;

    double get_best_obj() const override;

    double get_best_bound() const override;

    double get_relative_gap() const override;

    double get_absolute_gap() const override;

    double get_var_primal(const Var &t_var) const override;

    double get_var_reduced_cost(const Var &t_var) const override;

    double get_var_ray(const Var &t_var) const override;

    double get_ctr_dual(const Ctr &t_ctr) const override;

    double get_ctr_farkas(const Ctr &t_ctr) const override;

    unsigned int get_n_solutions() const override;

    unsigned int get_solution_index() const override;

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

#endif //IDOL_OPTIMIZERS_PESSIMISTICASOPTIMISTIC_H
