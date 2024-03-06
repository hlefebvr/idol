//
// Created by henri on 04.03.24.
//

#ifndef IDOL_OPTIMIZERS_BILEVEL_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_OPTIMIZERS_BILEVEL_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/optimizers/Algorithm.h"
#include "idol/optimizers/OptimizerFactory.h"
#include "idol/modeling/annotations/Annotation.h"
#include "idol/modeling/constraints/Ctr.h"
#include "MinMaxMinFormulation.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/ColumnAndConstraintGenerationSeparator.h"

namespace idol::Optimizers::Bilevel {
    class ColumnAndConstraintGeneration;
}

class idol::Optimizers::Bilevel::ColumnAndConstraintGeneration : public idol::Algorithm {
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_lower_level_optimizer;

    const Annotation<Var, unsigned int> m_lower_level_variables;
    const Annotation<Ctr, unsigned int> m_lower_level_constraints;
    const Ctr m_lower_level_objective;
    const bool m_use_extended_level_separator = true;

    std::optional<Annotation<Var, unsigned int>> m_variable_stage;
    std::optional<Annotation<Ctr, unsigned int>> m_constraint_stage;

    std::unique_ptr<idol::Bilevel::impl::MinMaxMinFormulation> m_formulation;
    std::unique_ptr<idol::Robust::ColumnAndConstraintGenerationSeparator> m_separator;
    std::unique_ptr<idol::Model> m_two_stage_robust_model;
public:
    ColumnAndConstraintGeneration(const Model& t_model,
                                  const Annotation<Var, unsigned int>& t_lower_level_variables,
                                  const Annotation<Ctr, unsigned int>& t_lower_level_constraints,
                                  Ctr t_lower_level_objective,
                                  const OptimizerFactory& t_master_optimizer,
                                  const OptimizerFactory& t_lower_level_optimizer);

    std::string name() const override;
    double get_var_primal(const Var &t_var) const override;
    double get_var_reduced_cost(const Var &t_var) const override;
    double get_var_ray(const Var &t_var) const override;
    double get_ctr_dual(const Ctr &t_ctr) const override;
    double get_ctr_farkas(const Ctr &t_ctr) const override;
    unsigned int get_n_solutions() const override;
    unsigned int get_solution_index() const override;

    const Annotation<Var, unsigned int> lower_level_variables() const { return m_lower_level_variables; }
    const Annotation<Ctr, unsigned int> lower_level_constraints() const { return m_lower_level_constraints; }
    const Ctr& lower_level_objective() const { return m_lower_level_objective; }
protected:
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
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

    double compute_penalty_parameter() const;
};

#endif //IDOL_OPTIMIZERS_BILEVEL_COLUMNANDCONSTRAINTGENERATION_H
