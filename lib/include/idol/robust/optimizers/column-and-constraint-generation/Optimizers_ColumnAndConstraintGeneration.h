//
// Created by henri on 11.12.24.
//

#ifndef IDOL_OPTIMIZERS_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_OPTIMIZERS_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/robust/modeling/Description.h"
#include "idol/general/optimizers/Algorithm.h"
#include "idol/general/optimizers/OptimizerFactory.h"
#include "Formulation.h"
#include "idol/bilevel/modeling/Description.h"

namespace idol::Optimizers::Robust {
    class ColumnAndConstraintGeneration;
}

class idol::Optimizers::Robust::ColumnAndConstraintGeneration : public Algorithm {

    const ::idol::Robust::Description &m_robust_description;
    const ::idol::Bilevel::Description &m_bilevel_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<idol::CCG::Formulation> m_formulation;
    unsigned int m_n_iterations = 0;

    // Initial scenarios
    std::vector<Point<Var>> m_initial_scenarios;
    std::unique_ptr<OptimizerFactory> m_initial_scenario_by_minimization;
    std::unique_ptr<OptimizerFactory> m_initial_scenario_by_maximization;
public:
    ColumnAndConstraintGeneration(const Model& t_parent,
                                  const ::idol::Robust::Description &t_robust_description,
                                  const ::idol::Bilevel::Description &t_bilevel_description,
                                  const OptimizerFactory &t_master_optimizer,
                                  std::vector<Point<Var>> t_initial_scenarios,
                                  OptimizerFactory* t_initial_scenario_by_minimization,
                                  OptimizerFactory* t_initial_scenario_by_maximization);

    [[nodiscard]] std::string name() const override;

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

    void hook_before_optimize() override;

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

    void add_initial_scenarios();
    void add_initial_scenario_by_min_or_max(const OptimizerFactory& t_optimizer, double t_coefficient);
    void solve_master_problem();
    void check_termination_criteria();
    void log_iteration();
};


#endif //IDOL_OPTIMIZERS_COLUMNANDCONSTRAINTGENERATION_H
