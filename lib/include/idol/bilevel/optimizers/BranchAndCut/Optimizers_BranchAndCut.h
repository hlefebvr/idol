//
// Created by henri on 11.04.25.
//

#ifndef IDOL_BILEVEL_OPTIMIZERS_BRANCHANDCUT_H
#define IDOL_BILEVEL_OPTIMIZERS_BRANCHANDCUT_H

#include "idol/general/optimizers/Algorithm.h"
#include "idol/bilevel/modeling/Description.h"
#include "idol/general/utils/GenerationPattern.h"

namespace idol::Optimizers::Bilevel {
    class BranchAndCut;
}

class idol::Optimizers::Bilevel::BranchAndCut : public Algorithm {
    const ::idol::Bilevel::Description& m_description;
    std::unique_ptr<OptimizerFactory> m_optimizer_for_sub_problems;
    std::unique_ptr<Model> m_hpr;
    std::unique_ptr<Model> m_separation_problem;
    std::vector<Var> m_linking_upper_variables;
    std::optional<Var> m_pi;
    std::vector<Var> m_lambda;

    void check_value_function_is_well_posed();
    void check_assumptions();
    void build_separation_problem();
    std::tuple<double, double, double, double> compute_big_M(const Model& t_model);
    void build_hpr(double t_lambda_lb, double t_lambda_ub, double t_pi_lb, double t_pi_ub);
    GenerationPattern<Ctr> build_dantzig_wolfe_cut();

    class BoundTighteningCallback;
public:
    BranchAndCut(const Model& t_parent,
                 const ::idol::Bilevel::Description& t_description,
                 const OptimizerFactory& t_optimizer_for_sub_problems);

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
};

#endif //IDOL_BILEVEL_OPTIMIZERS_BRANCHANDCUT_H
