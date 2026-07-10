//
// Created by Henri on 04/06/2026.
//

#ifndef IDOL_OPTIMIZERS_MAXMINRELAXATION_H
#define IDOL_OPTIMIZERS_MAXMINRELAXATION_H

#include "idol/bilevel/modeling/Description.h"
#include "idol/general/optimizers/Algorithm.h"
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/robust/modeling/Description.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

#define THROW_NOT_IMPLEMENTED { throw Exception("Called function " + std::string(__FUNCTION__) + " in " + std::string(__FILE__) + ":" + std::to_string(__LINE__) + ", which is not implemented."); }

namespace idol::Optimizers::Robust {
    class MaxMinRelaxation;
}

class idol::Optimizers::Robust::MaxMinRelaxation : public Algorithm {
    const idol::Robust::Description& m_description;
    const idol::Bilevel::Description& m_bilevel_description;
    std::unique_ptr<OptimizerFactory> m_master_optimizer_factory;
    std::unique_ptr<OptimizerFactory> m_deterministic_optimizer_factory;
    const bool m_use_indicator;
    const std::list<PrimalPoint> m_initial_scenarios;

    struct Formulation {
        Model model;
        idol::Robust::Description description;

        Formulation(Model&& t_hpr, idol::Robust::Description&& t_description);
    };

    std::unique_ptr<Formulation> m_formulation;
public:
    MaxMinRelaxation(const Model& t_model,
                     const idol::Robust::Description& t_description,
                     const idol::Bilevel::Description& t_bilevel_description,
                     const OptimizerFactory& t_master_optimizer,
                     const OptimizerFactory& t_deterministic_optimizer,
                     bool t_use_indicator,
                     const std::list<PrimalPoint>& t_initial_scenarios
    );

    [[nodiscard]] std::string name() const override { return "max-min relaxation"; }
    [[nodiscard]] double get_var_primal(const Var& t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var& t_var) const override THROW_NOT_IMPLEMENTED
    [[nodiscard]] double get_var_ray(const Var& t_var) const override THROW_NOT_IMPLEMENTED
    [[nodiscard]] double get_ctr_dual(const Ctr& t_ctr) const override THROW_NOT_IMPLEMENTED
    [[nodiscard]] double get_ctr_farkas(const Ctr& t_ctr) const override THROW_NOT_IMPLEMENTED
    [[nodiscard]] unsigned get_n_solutions() const override THROW_NOT_IMPLEMENTED
    [[nodiscard]] unsigned get_solution_index() const override THROW_NOT_IMPLEMENTED

    [[nodiscard]] const idol::Robust::Description& description() const { return m_description; }
    [[nodiscard]] const OptimizerFactory& get_master_optimizer_factory() const { return *m_master_optimizer_factory; }
    [[nodiscard]] const OptimizerFactory& get_deterministic_optimizer_factory() const { return *m_deterministic_optimizer_factory; }
    [[nodiscard]] bool use_indicator() const { return m_use_indicator; }
    [[nodiscard]] const Formulation& get_formulation() const { return *m_formulation; }

    void build_model();
protected:
    void add(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void add(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void add(const QCtr& t_ctr) override THROW_NOT_IMPLEMENTED
    void add(const SOSCtr& t_ctr) override THROW_NOT_IMPLEMENTED
    void remove(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void remove(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void remove(const QCtr& t_ctr) override THROW_NOT_IMPLEMENTED
    void remove(const SOSCtr& t_ctr) override THROW_NOT_IMPLEMENTED
    void update() override {}
    void write(const std::string& t_name) override THROW_NOT_IMPLEMENTED
    void hook_before_optimize() override;
    void hook_optimize() override;
    void set_solution_index(unsigned t_index) override THROW_NOT_IMPLEMENTED
    void update_obj_sense() override {}
    void update_obj() override {}
    void update_rhs() override THROW_NOT_IMPLEMENTED
    void update_obj_constant() override THROW_NOT_IMPLEMENTED
    void update_mat_coeff(const Ctr& t_ctr, const Var& t_var) override THROW_NOT_IMPLEMENTED
    void update_ctr_type(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void update_ctr_rhs(const Ctr& t_ctr) override THROW_NOT_IMPLEMENTED
    void update_var_type(const Var& t_var) override THROW_NOT_IMPLEMENTED
    void update_var_lb(const Var& t_var) override;
    void update_var_ub(const Var& t_var) override;
    void update_var_obj(const Var& t_var) override THROW_NOT_IMPLEMENTED

    void throw_if_no_formulation() const;
};

#endif //IDOL_OPTIMIZERS_MAXMINRELAXATION_H