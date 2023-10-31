//
// Created by henri on 31.10.23.
//

#ifndef IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
#define IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H

#include "idol/optimizers/Algorithm.h"
#include "DantzigWolfeFormulation.h"
#include "DantzigWolfeSubProblem.h"

namespace idol::Optimizers {
    class DantzigWolfeDecomposition;
}

class idol::Optimizers::DantzigWolfeDecomposition : public Algorithm {
    idol::DantzigWolfe::Formulation m_formulation;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::vector<DantzigWolfe::SubProblem> m_sub_problem_specifications;
    unsigned int m_max_parallel_pricing;

    class ColumnGeneration;
public:
    DantzigWolfeDecomposition(const Model& t_model,
                              idol::DantzigWolfe::Formulation&& t_formulation,
                              const OptimizerFactory& t_master_optimizer_factory,
                              unsigned int t_max_parallel_pricing,
                              std::vector<DantzigWolfe::SubProblem>&& t_sub_problem_specifications);

    std::string name() const override;
protected:
    void hook_before_optimize() override;
    void hook_optimize() override;
    void hook_after_optimize() override;

    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;
    double get_var_primal(const Var &t_var) const override;
    double get_var_ray(const Var &t_var) const override;
    double get_ctr_dual(const Ctr &t_ctr) const override;
    double get_ctr_farkas(const Ctr &t_ctr) const override;
    unsigned int get_n_solutions() const override;
    unsigned int get_solution_index() const override;
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

#endif //IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
