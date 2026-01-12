//
// Created by henri on 09.01.26.
//

#ifndef IDOL_OPTIMIZERS_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H
#define IDOL_OPTIMIZERS_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H

#include "idol/general/optimizers/Algorithm.h"
#include "idol/robust/modeling/Description.h"
#include "Formulation.h"

namespace idol::Optimizers::Robust {
    class CriticalValueColumnAndConstraintGeneration;
}

class idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration : public Algorithm {
    const idol::Robust::Description& m_robust_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer;
    std::unique_ptr<OptimizerFactory> m_separation_optimizer;

    std::unique_ptr<idol::CVCCG::Formulation> m_formulation;

    std::optional<PrimalPoint> m_last_master_solution;
    unsigned int m_n_iterations = 0;
protected:
    void add(const Var& t_var) override;
    void add(const Ctr& t_ctr) override;
    void add(const QCtr& t_ctr) override;
    void add(const SOSCtr& t_ctr) override;
    void remove(const Var& t_var) override;
    void remove(const Ctr& t_ctr) override;
    void remove(const QCtr& t_ctr) override;
    void write(const std::string& t_name) override;
    void hook_before_optimize() override;
    void hook_optimize() override;
    void hook_after_optimize() override;
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
    void update() override;

    idol::CVCCG::Formulation& get_formulation_or_throw();
    [[nodiscard]] const idol::CVCCG::Formulation& get_formulation_or_throw() const;

    void solve_master_problem();
    void solve_separation_problem();
    void add_scenario_to_master_problem();
public:
    CriticalValueColumnAndConstraintGeneration(const Model& t_parent,
        const idol::Robust::Description& t_description,
        const OptimizerFactory& t_master_optimizer,
        const OptimizerFactory& t_separation_optimizer
    );

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] double get_var_primal(const Var& t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const override;
    [[nodiscard]] double get_var_ray(const Var& t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const override;
    [[nodiscard]] unsigned get_n_solutions() const override;
    [[nodiscard]] unsigned get_solution_index() const override;

    friend class idol::CVCCG::Formulation;
};

#endif //IDOL_OPTIMIZERS_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H