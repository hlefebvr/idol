//
// Created by henri on 12.04.25.
//

#ifndef IDOL_OPTIMIZERS_SCENARIOBASEDKADAPTABILITYPROBLEMSOLVER_H
#define IDOL_OPTIMIZERS_SCENARIOBASEDKADAPTABILITYPROBLEMSOLVER_H


#include "idol/general/optimizers/Algorithm.h"
#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/robust/modeling/Description.h"

namespace idol::Optimizers::Robust {
    class ScenarioBasedKAdaptabilityProblemSolver;
}

class idol::Optimizers::Robust::ScenarioBasedKAdaptabilityProblemSolver : public Algorithm {
    unsigned int m_K;
    std::unique_ptr<OptimizerFactory> m_main_optimizer;
    std::unique_ptr<OptimizerFactory> m_separation_optimizer;
    const ::idol::Bilevel::Description& m_bilevel_description;
    const ::idol::Robust::Description& m_robust_description;

    bool m_has_first_stage_decisions = true;
    unsigned int m_solution_index = 0;

    std::optional<Var> m_epigraph_variable;
    std::vector<std::vector<std::optional<Var>>> m_y_k;
    std::vector<std::unique_ptr<Model>> m_master_problem;
    std::list<Ctr> m_uncertain_constraints;
    QuadExpr<Var> m_second_stage_objective;

    std::vector<std::list<Ctr>> m_scenario_based_constraints;

    [[nodiscard]] const Var& get_kadapt_var(const idol::Var &t_var, unsigned int t_k) const;
    void build_master_if_not_exists();
    void make_second_stage_variables_duplicates();
    void make_epigraph_variable_if_not_exists();
    void add_variables_to_master();
    void add_deterministic_constraints_to_master();
    void add_first_stage_objective();
    void set_uncertainty_disjunction(unsigned int t_k, const std::list<PrimalPoint>& t_disjunction);
public:
    ScenarioBasedKAdaptabilityProblemSolver(const Model& t_parent,
                                            const idol::Bilevel::Description& t_bilevel_description,
                                            const idol::Robust::Description& t_robust_description,
                                            unsigned int t_K,
                                            const OptimizerFactory& t_main_optimizer,
                                            const OptimizerFactory& t_separation_optimizer);

    [[nodiscard]] std::string name() const override;
    [[nodiscard]] double get_var_primal(const Var &t_var) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var &t_var) const override;
    [[nodiscard]] double get_var_ray(const Var &t_var) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr &t_ctr) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr &t_ctr) const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;

    void set_uncertainty_disjunction(const std::vector<std::list<PrimalPoint>>& t_disjunctions);
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
};

#endif //IDOL_OPTIMIZERS_SCENARIOBASEDKADAPTABILITYPROBLEMSOLVER_H
