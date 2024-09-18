//
// Created by henri on 18.09.24.
//

#ifndef IDOL_OPTIMIZERS_ALTERNATINGDIRECTIONMETHOD_H
#define IDOL_OPTIMIZERS_ALTERNATINGDIRECTIONMETHOD_H

#include "idol/optimizers/Algorithm.h"
#include "idol/optimizers/mixed-integer-optimization/padm/SubProblem.h"
#include "Formulation.h"

namespace idol::Optimizers {
    class AlternatingDirectionMethod;
}

class idol::Optimizers::AlternatingDirectionMethod : public Algorithm {
public:
    AlternatingDirectionMethod(const Model& t_model,
                               AlternatingDirection::Formulation t_formulation,
                               std::vector<idol::AlternatingDirection::SubProblem>&& t_sub_problem_specs);

    std::string name() const override { return "AlternatingDirectionMethod"; }

    double get_var_primal(const Var &t_var) const override;

    double get_var_reduced_cost(const Var &t_var) const override;

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

    void update_penalty_parameters();
    void run_inner_loop();
    bool is_feasible() const;
    bool is_feasible(unsigned int t_sub_problem_id) const;
    bool solve_sub_problem(unsigned int t_sub_problem_id);
private:
    AlternatingDirection::Formulation m_formulation;
    std::vector<idol::AlternatingDirection::SubProblem> m_sub_problem_specs;
    unsigned int m_max_inner_loop_iterations = 1000;

    unsigned int m_outer_loop_iteration = 0;
    unsigned int m_inner_loop_iterations = 0;
    std::vector<Solution::Primal> m_last_solutions;
};


#endif //IDOL_OPTIMIZERS_ALTERNATINGDIRECTIONMETHOD_H
