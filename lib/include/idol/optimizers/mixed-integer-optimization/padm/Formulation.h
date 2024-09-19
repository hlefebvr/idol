//
// Created by henri on 18.09.24.
//

#ifndef IDOL_ADM_FORMULATION_H
#define IDOL_ADM_FORMULATION_H

#include "idol/modeling/models/Model.h"
#include "PenaltyUpdates.h"

namespace idol::ADM {
    class Formulation;
}

class idol::ADM::Formulation {
public:
    Formulation(const Model& t_src_model,
                Annotation<Var, unsigned int> t_decomposition,
                std::optional<Annotation<Ctr, bool>> t_penalized_constraints);

    Model& sub_problem(const Var& t_var);

    const Model& sub_problem(const Var& t_var) const;

    Model& sub_problem(unsigned int t_sub_problem_id) { return m_sub_problems[t_sub_problem_id]; }

    const Model& sub_problem(unsigned int t_sub_problem_id) const { return m_sub_problems[t_sub_problem_id]; }

    unsigned int n_sub_problems() const { return m_sub_problems.size(); }

    auto sub_problems() { return IteratorForward(m_sub_problems); }

    auto sub_problems() const { return ConstIteratorForward(m_sub_problems); }

    const auto l1_vars(unsigned int t_sub_problem_id) const { return ConstIteratorForward(m_l1_vars[t_sub_problem_id]); }

    bool has_penalized_constraints() const { return m_penalized_constraints.has_value(); }

    void fix_sub_problem(unsigned int t_sub_problem_id, const std::vector<Solution::Primal>& t_primals);

    void update_penalty_parameters(const std::vector<Solution::Primal>& t_primals, PenaltyUpdate& t_penalty_update);
private:
    Annotation<Var, unsigned int> m_decomposition;
    std::optional<Annotation<Ctr, bool>> m_penalized_constraints;

    std::vector<Model> m_sub_problems;
    std::vector<std::optional<Expr<Var, Var>>> m_objective_patterns;
    std::vector<std::list<std::pair<Ctr, Expr<Var, Var>>>> m_constraint_patterns; // as ctr: lhs <= 0
    std::vector<std::list<Var>> m_l1_vars;

    unsigned int compute_n_sub_problems(const Model& t_src_model) const;
    void initialize_sub_problems(const Model& t_src_model, unsigned int n_sub_problems);
    void initialize_patterns(const Model& t_src_model, unsigned int n_sub_problems);
    void initialize_slacks(const Model& t_src_model, unsigned int n_sub_problems);
    void dispatch_vars(const Model& t_src_model);
    void dispatch_ctrs(const Model& t_src_model);
    void dispatch_ctr(const Model& t_src_model, const Ctr& t_ctr, unsigned int t_sub_problem_id);
    void dispatch_obj(const Model& t_src_model);
    void dispatch_obj(const Model& t_src_model, unsigned int t_sub_problem_id);
    std::pair<Expr<Var, Var>, bool> dispatch(const Model& t_src_model, const LinExpr<Var>& t_lin_expr, const QuadExpr<Var, Var>& t_quad_expr, unsigned int t_sub_problem_id);

    double fix(const Constant& t_constant, const std::vector<Solution::Primal>& t_primals);
};


#endif //IDOL_ADM_FORMULATION_H
