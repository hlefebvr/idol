//
// Created by henri on 08.02.24.
//

#ifndef IDOL_OPTIMIZERS_ROBUST_COLUMNANDCONSTRAINTGENERATION_H
#define IDOL_OPTIMIZERS_ROBUST_COLUMNANDCONSTRAINTGENERATION_H

#include "idol/optimizers/Algorithm.h"
#include "idol/optimizers/OptimizerFactory.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/CCGSeparator.h"
#include "idol/modeling/models/Model.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/CCGStabilizer.h"

#include <optional>

namespace idol::Optimizers::Robust {
    class ColumnAndConstraintGeneration;
}

class idol::Optimizers::Robust::ColumnAndConstraintGeneration : public Algorithm {
    std::unique_ptr<idol::Robust::CCGSeparator> m_separator;
    std::unique_ptr<idol::Robust::CCGStabilizer::Strategy> m_stabilizer;

    friend idol::Robust::CCGStabilizer::Strategy;

    Model m_master_problem;
    Model m_uncertainty_set;
    const Annotation<Var, unsigned int> m_variable_stage;
    const Annotation<Ctr, unsigned int> m_constraint_stage;
    bool m_complete_recourse;

    Timer m_separation_timer;
    unsigned int m_iteration_count = 0;
    std::optional<Solution::Primal> m_current_separation_solution;
    std::optional<Solution::Primal> m_current_master_solution;

    std::optional<std::pair<Var, Ctr>> m_epigraph;
    std::list<Ctr> m_coupling_constraints;
    std::list<Var> m_upper_level_variables_list;
    std::list<Var> m_lower_level_variables_list;
    std::list<Ctr> m_upper_level_constraints_list;
    std::list<Ctr> m_lower_level_constraints_list;
public:
    ColumnAndConstraintGeneration(const Model& t_parent,
                                  const Model& t_uncertainty_set,
                                  const OptimizerFactory& t_master_optimizer,
                                  const idol::Robust::CCGSeparator& t_separator,
                                  const idol::Robust::CCGStabilizer& t_stabilizer,
                                  const Annotation<Var, unsigned int>& t_lower_level_variables,
                                  const Annotation<Ctr, unsigned int>& t_lower_level_constraints,
                                  bool t_complete_recourse
                                  );

    std::string name() const override;
    double get_var_primal(const Var &t_var) const override;
    double get_var_reduced_cost(const Var &t_var) const override;
    double get_var_ray(const Var &t_var) const override;
    double get_ctr_dual(const Ctr &t_ctr) const override;
    double get_ctr_farkas(const Ctr &t_ctr) const override;
    unsigned int get_n_solutions() const override;
    unsigned int get_solution_index() const override;

    const Model& master_problem() const { return m_master_problem; }
    const Model& uncertainty_set() const { return m_uncertainty_set; }
    const Annotation<Ctr, unsigned int> lower_level_constraints() const { return m_constraint_stage; };
    const Annotation<Var, unsigned int> lower_level_variables() const { return m_variable_stage; };
    bool complete_recourse() const { return m_complete_recourse; }
protected:
    void build_master_problem();
    void build_master_problem_variables();
    void build_master_problem_constraints();
    void build_master_problem_objective_and_epigraph();
    void build_coupling_constraints_list();
    void build_upper_and_lower_level_variables_list();
    void build_upper_and_lower_level_constraints_list();

    bool contains_lower_level_variable(const LinExpr<Var>& t_expr);
    bool contains_lower_level_variable(const QuadExpr<Var, Var>& t_expr);

    Solution::Primal solve_master_problem();
    Solution::Primal solve_separation_problems();
    void add_scenario(const Solution::Primal& t_most_violated_scenario);
    void check_stopping_condition();

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

    void log_init() const;
    void log(bool t_is_rmp) const;
};

#endif //IDOL_OPTIMIZERS_ROBUST_COLUMNANDCONSTRAINTGENERATION_H
