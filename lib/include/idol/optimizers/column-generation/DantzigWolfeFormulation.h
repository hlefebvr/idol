//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFEFORMULATION_H
#define IDOL_DANTZIGWOLFEFORMULATION_H

#include "idol/modeling/models/Model.h"

namespace idol::DantzigWolfe {
    class Formulation;
}

class idol::DantzigWolfe::Formulation {
    Annotation<Ctr, unsigned int> m_decomposition_by_ctr;
    Annotation<Var, unsigned int> m_decomposition_by_var;

    Model m_master;
    std::vector<Model> m_sub_problems;
    std::vector<Column> m_generation_patterns;

    unsigned int compute_n_sub_problems(const Model& t_original_formulation);
    void set_decomposition_by_var(const Model& t_original_formulation);
    void init_sub_problems(unsigned int t_n_sub_problems);
    void init_generation_patterns(unsigned int t_n_sub_problems);
    void dispatch_variables(const Model& t_original_formulation);
    void dispatch_constraints(const Model& t_original_formulation);
    void dispatch_linking_constraint(const Ctr& t_original_ctr, const Row& t_row, CtrType t_type);
    std::pair<Expr<Var, Var>, std::vector<Constant>> decompose_expression(const LinExpr<Var> &t_linear, const QuadExpr<Var, Var>& t_quadratic);
    void dispatch_objective_function(const Model& t_original_formulation);
public:
    Formulation(const Model& t_original_formulation, Annotation<Ctr, unsigned int> t_decomposition);

    Model& get_model(const Var& t_var);
    const Model& get_model(const Var& t_var) const;

    Model& get_model(const Ctr& t_ctr);
    const Model& get_model(const Ctr& t_ctr) const;

    unsigned int n_sub_problems() const { return m_sub_problems.size(); }

    void add_aggregation_constraint(unsigned int t_sub_problem_id, double t_lower_multiplicity, double t_upper_multiplicity);
};

#endif //IDOL_DANTZIGWOLFEFORMULATION_H
