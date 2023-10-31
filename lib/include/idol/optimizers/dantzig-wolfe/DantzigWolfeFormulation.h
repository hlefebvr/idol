//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFEFORMULATION_H
#define IDOL_DANTZIGWOLFEFORMULATION_H

#include "idol/modeling/models/Model.h"
#include "idol/containers/GeneratorPool.h"

namespace idol::DantzigWolfe {
    class Formulation;
}

class idol::DantzigWolfe::Formulation {
    using PresentGeneratorsList = std::list<std::pair<Var, const Solution::Primal&>>;

    Annotation<Ctr, unsigned int> m_decomposition_by_ctr;
    Annotation<Var, unsigned int> m_decomposition_by_var;

    Model m_master;
    std::vector<Model> m_sub_problems;
    std::vector<Column> m_generation_patterns;
    std::vector<GeneratorPool<Var>> m_pools;
    std::vector<PresentGeneratorsList> m_present_generators;

    Map<Var, Ctr> m_soft_branching_lower_bound_constraints;
    Map<Var, Ctr> m_soft_branching_upper_bound_constraints;

    unsigned int compute_n_sub_problems(const Model& t_original_formulation);
    void set_decomposition_by_var(const Model& t_original_formulation);
    void initialize_sub_problems(unsigned int t_n_sub_problems);
    void initialize_generation_patterns(unsigned int t_n_sub_problems);
    void initialize_pools(unsigned int t_n_sub_problems);
    void initialize_present_generators(unsigned int t_n_sub_problems);
    void dispatch_variables(const Model& t_original_formulation);
    void dispatch_constraints(const Model& t_original_formulation);
    void dispatch_linking_constraint(const Ctr& t_original_ctr, const Row& t_row, CtrType t_type);
    std::pair<Expr<Var, Var>, std::vector<Constant>> decompose_expression(const LinExpr<Var> &t_linear, const QuadExpr<Var, Var>& t_quadratic);
    void dispatch_objective_function(const Model& t_original_formulation);

    void apply_sub_problem_bound_on_master(bool t_is_lb, const idol::Var &t_var, unsigned int t_sub_problem_id, double t_value);
    LinExpr<Var> reformulate_sub_problem_variable(const Var &t_var, unsigned int t_sub_problem_id);
public:
    Formulation(const Model& t_original_formulation, Annotation<Ctr, unsigned int> t_decomposition);

    Model& master() { return m_master; }
    const Model& master() const { return m_master; }

    auto sub_problems() { return IteratorForward(m_sub_problems); }
    auto sub_problems() const { return ConstIteratorForward(m_sub_problems); }

    auto present_generators(unsigned int t_id) const { return ConstIteratorForward(m_present_generators[t_id]); }

    Model& sub_problem(unsigned int t_id) { return m_sub_problems[t_id]; }
    const Model& sub_problem(unsigned int t_id) const { return m_sub_problems[t_id]; }

    Model& get_model(const Var& t_var);
    const Model& get_model(const Var& t_var) const;

    Model& get_model(const Ctr& t_ctr);
    const Model& get_model(const Ctr& t_ctr) const;

    unsigned int n_sub_problems() const { return m_sub_problems.size(); }

    void add_aggregation_constraint(unsigned int t_sub_problem_id, double t_lower_multiplicity, double t_upper_multiplicity);

    void update_sub_problem_objective(unsigned int t_sub_problem_id, const Solution::Dual& t_master_dual, bool t_use_farkas = false);

    void generate_column(unsigned int t_sub_problem_id, Solution::Primal t_generator);

    double compute_reduced_cost(unsigned int t_sub_problem_id, const Solution::Dual& t_master_dual, const Solution::Primal& t_generator);

    double get_original_space_var_primal(const Var& t_var, const Solution::Primal& t_master_primal) const;

    void update_var_lb(const Var& t_var, double t_lb, bool t_hard);

    void update_var_ub(const Var& t_var, double t_ub, bool t_hard);

    void remove_column_if(unsigned int t_sub_problem_id, const std::function<bool(const Var &, const Solution::Primal &)> &t_indicator_for_removal);

    void update_obj(const Expr<Var, Var>& t_expr);
};

#endif //IDOL_DANTZIGWOLFEFORMULATION_H
