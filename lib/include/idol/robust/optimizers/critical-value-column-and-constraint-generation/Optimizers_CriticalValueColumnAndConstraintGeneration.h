//
// Created by Henri on 17/04/2026.
//

#ifndef IDOL_OPTIMIZERS_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H
#define IDOL_OPTIMIZERS_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H

#include "Formulation.h"
#include "idol/general/optimizers/Algorithm.h"
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Optimizers::Robust {
    class CriticalValueColumnAndConstraintGeneration;
}

class idol::Optimizers::Robust::CriticalValueColumnAndConstraintGeneration : public Algorithm {
    const idol::Robust::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer_factory;
    std::unique_ptr<CVCCG::Formulation> m_formulation;
public:
    CriticalValueColumnAndConstraintGeneration(const Model& t_model,
                                               const idol::Robust::Description& t_description,
                                               const OptimizerFactory& t_master_optimizer,
                                               const OptimizerFactory& t_deterministic_optimizer
    );

protected:
    void add(const Var& t_var) override { m_formulation.reset(); }
    void add(const Ctr& t_ctr) override { m_formulation.reset(); }
    void add(const QCtr& t_ctr) override { m_formulation.reset(); }
    void add(const SOSCtr& t_ctr) override { m_formulation.reset(); }
    void remove(const Var& t_var) override { m_formulation.reset(); }
    void remove(const Ctr& t_ctr) override { m_formulation.reset(); }
    void remove(const QCtr& t_ctr) override { m_formulation.reset(); }
    void remove(const SOSCtr& t_ctr) override { m_formulation.reset(); }
    void update() override { m_formulation.reset(); }
    void write(const std::string& t_name) override;
    void hook_optimize() override;
    void set_solution_index(unsigned t_index) override;
    void update_obj_sense() override { m_formulation.reset(); }
    void update_obj() override { m_formulation.reset(); }
    void update_rhs() override { m_formulation.reset(); }
    void update_obj_constant() override { m_formulation.reset(); }
    void update_mat_coeff(const Ctr& t_ctr, const Var& t_var) override { m_formulation.reset(); }
    void update_ctr_type(const Ctr& t_ctr) override { m_formulation.reset(); }
    void update_ctr_rhs(const Ctr& t_ctr) override { m_formulation.reset(); }
    void update_var_type(const Var& t_var) override { m_formulation.reset(); }
    void update_var_lb(const Var& t_var) override { m_formulation.reset(); }
    void update_var_ub(const Var& t_var) override { m_formulation.reset(); }
    void update_var_obj(const Var& t_var) override { m_formulation.reset(); }

    void solve_master_problem();
    void analyze_master_problem();
    bool check_stopping_criterion();
    void solve_sub_problems();
public:
    [[nodiscard]] std::string name() const override { return "CVCCG"; }
    [[nodiscard]] double get_var_primal(const Var& t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const override;
    [[nodiscard]] double get_var_ray(const Var& t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const override;
    [[nodiscard]] unsigned get_n_solutions() const override;
    [[nodiscard]] unsigned get_solution_index() const override;

    [[nodiscard]] const idol::Robust::Description& description() const { return m_description; }
    [[nodiscard]] const OptimizerFactory& get_master_optimizer_factory() const { return *m_master_optimizer_factory; }
    [[nodiscard]] const OptimizerFactory& get_deterministic_optimizer_factory() const { return *m_deterministic_optimizer_factory; }
};

#endif //IDOL_OPTIMIZERS_CRITICALVALUECOLUMNANDCONSTRAINTGENERATION_H