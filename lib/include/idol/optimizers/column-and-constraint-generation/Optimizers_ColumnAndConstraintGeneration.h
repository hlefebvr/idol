//
// Created by henri on 08.02.24.
//

#ifndef IDOL_OPTIMIZERS_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_OPTIMIZERS_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/optimizers/Algorithm.h"
#include "idol/optimizers/OptimizerFactory.h"
#include "ColumnAndConstraintGenerationSeparator.h"
#include "idol/modeling/models/Model.h"

namespace idol::Optimizers {
    class ColumnAndConstraintGeneration;
}

class idol::Optimizers::ColumnAndConstraintGeneration : public Algorithm {
    Model m_uncertainty_set;
    const Annotation<Var, unsigned int> m_lower_level_variables;
    const Annotation<Ctr, unsigned int> m_lower_level_constraints;
public:
    ColumnAndConstraintGeneration(const Model& t_parent,
                                  const Model& t_uncertainty_set,
                                  const OptimizerFactory& t_master_optimizer,
                                  const ColumnAndConstraintGenerationSeparator& t_separator,
                                  const Annotation<Var, unsigned int>& t_lower_level_variables,
                                  const Annotation<Ctr, unsigned int>& t_lower_level_constraints
                                  );

    std::string name() const override;
    double get_var_primal(const Var &t_var) const override;
    double get_var_ray(const Var &t_var) const override;
    double get_ctr_dual(const Ctr &t_ctr) const override;
    double get_ctr_farkas(const Ctr &t_ctr) const override;
    unsigned int get_n_solutions() const override;
    unsigned int get_solution_index() const override;
protected:
    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
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

#endif //IDOL_OPTIMIZERS_COLUMNANDCONSTRAINTGENERATION_H
