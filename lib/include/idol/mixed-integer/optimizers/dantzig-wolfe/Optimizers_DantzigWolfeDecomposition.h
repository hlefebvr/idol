//
// Created by henri on 31.10.23.
//

#ifndef IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
#define IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H

#include "idol/general/optimizers/Algorithm.h"
#include "Formulation.h"
#include "SubProblem.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/DualPriceSmoothingStabilization.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/logs/Factory.h"

namespace idol::Optimizers {
    class DantzigWolfeDecomposition;
}

class idol::Optimizers::DantzigWolfeDecomposition : public Algorithm {
    idol::DantzigWolfe::Formulation m_formulation;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<DantzigWolfe::InfeasibilityStrategyFactory::Strategy> m_strategy;
    std::unique_ptr<DantzigWolfe::DualPriceSmoothingStabilization::Strategy> m_stabilization;
    std::unique_ptr<Logs::DantzigWolfe::Factory::Strategy> m_logger;
    std::optional<DantzigWolfe::SubProblem> m_default_sub_problem_spec;
    std::vector<DantzigWolfe::SubProblem> m_sub_problem_specifications;
    unsigned int m_with_dynamic_sub_problems = true;
    unsigned int m_max_parallel_pricing;
    bool m_use_hard_branching;
    bool m_remove_infeasible_columns;
public:
    DantzigWolfeDecomposition(const Model& t_model,
                              idol::DantzigWolfe::Formulation&& t_formulation,
                              const OptimizerFactory& t_master_optimizer_factory,
                              const DantzigWolfe::DualPriceSmoothingStabilization& t_stabilization,
                              unsigned int t_max_parallel_pricing,
                              bool t_use_hard_branching,
                              bool t_remove_infeasible_columns,
                              std::vector<DantzigWolfe::SubProblem>&& t_sub_problem_specifications,
                              std::optional<DantzigWolfe::SubProblem> t_default_sub_problem_spec,
                              const DantzigWolfe::InfeasibilityStrategyFactory& t_strategy,
                              const Logs::DantzigWolfe::Factory& t_logger_factory);

    std::string name() const override;

    [[nodiscard]] const DantzigWolfe::Formulation& formulation() const { return m_formulation; }

    DantzigWolfe::Formulation& formulation() { return m_formulation; }

    class ColumnGeneration;
protected:
    void hook_before_optimize() override;
    void hook_optimize() override;
    void hook_after_optimize() override;

    void add(const Var &t_var) override;
    void add(const Ctr &t_ctr) override;
    void add(const QCtr &t_ctr) override;
    void remove(const Var &t_var) override;
    void remove(const Ctr &t_ctr) override;
    void remove(const QCtr &t_ctr) override;
    void update() override;
    void write(const std::string &t_name) override;
    double get_var_primal(const Var &t_var) const override;
    double get_var_reduced_cost(const Var &t_var) const override;
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
    void add_sub_problem();
};

#endif //IDOL_OPTIMIZERS_DANTZIGWOLFEDECOMPOSITION_H
