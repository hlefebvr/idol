//
// Created by henri on 18.09.24.
//

#ifndef IDOL_ADM_FORMULATION_H
#define IDOL_ADM_FORMULATION_H

#include <utility>

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/general/utils/Map.h"

namespace idol {
    class PenaltyUpdate;
    namespace ADM {
        class Formulation;
    }
}

class idol::ADM::Formulation {
public:
    Formulation(const Model& t_src_model,
                Annotation<Var, unsigned int> t_decomposition,
                std::optional<Annotation<Ctr, bool>> t_penalized_constraints,
                bool t_independent_penalty_update,
                std::pair<bool, double> t_rescaling);

    Model& sub_problem(const Var& t_var);

    const Model& sub_problem(const Var& t_var) const;

    unsigned int sub_problem_id(const Var& t_var) const;

    Model& sub_problem(unsigned int t_sub_problem_id) { return m_sub_problems[t_sub_problem_id]; }

    const Model& sub_problem(unsigned int t_sub_problem_id) const { return m_sub_problems[t_sub_problem_id]; }

    unsigned int n_sub_problems() const { return m_sub_problems.size(); }

    auto sub_problems() { return IteratorForward(m_sub_problems); }

    auto sub_problems() const { return ConstIteratorForward(m_sub_problems); }

    auto l1_vars(unsigned int t_sub_problem_id) const { return ConstIteratorForward(m_l1_vars_in_sub_problem[t_sub_problem_id]); }

    bool has_penalized_constraints() const { return m_penalized_constraints.has_value(); }

    void fix_sub_problem(unsigned int t_sub_problem_id, const std::vector<PrimalPoint>& t_primals);

    void initialize_penalty_parameters(double t_value);

    bool update_penalty_parameters(const std::vector<PrimalPoint>& t_primals, PenaltyUpdate& t_penalty_update); // Returns true if penalty parameters have been resacled

    struct CurrentPenalty {
        const Ctr constraint;
        const Var variable;
        const double max_violation;
        double penalty;
        CurrentPenalty(Ctr t_constraint, Var t_variable, double t_max_violation, double t_penalty)
            : constraint(std::move(t_constraint)), variable(std::move(t_variable)), max_violation(t_max_violation), penalty(t_penalty) {}
    };

private:
    Annotation<Var, unsigned int> m_decomposition;
    std::optional<Annotation<Ctr, bool>> m_penalized_constraints;
    bool m_independent_penalty_update;
    std::pair<bool, double> m_rescaling;

    std::vector<Model> m_sub_problems;
    std::vector<std::optional<Expr<Var, Var>>> m_objective_patterns;
    std::vector<std::list<std::pair<Ctr, Expr<Var, Var>>>> m_constraint_patterns; // as ctr: row <= 0
    std::vector<std::list<Var>> m_l1_vars_in_sub_problem;
    Map<Ctr, Var> m_l1_vars;

    unsigned int compute_n_sub_problems(const Model& t_src_model) const;
    void initialize_sub_problems(const Model& t_src_model, unsigned int n_sub_problems);
    void initialize_patterns(const Model& t_src_model, unsigned int n_sub_problems);
    void initialize_slacks(const Model& t_src_model, unsigned int n_sub_problems);
    void dispatch_vars(const Model& t_src_model);
    void dispatch_ctrs(const Model& t_src_model);
    void dispatch_ctr(const Model& t_src_model, const Ctr& t_ctr, unsigned int t_sub_problem_id);
    void dispatch_obj(const Model& t_src_model);
    void dispatch_obj(const Model& t_src_model, unsigned int t_sub_problem_id);
    std::pair<Expr<Var, Var>, bool> dispatch(const Model& t_src_model, const LinExpr<Var>& t_lin_expr, /* const QuadExpr<Var, Var>& t_quad_expr, */ unsigned int t_sub_problem_id);
    Var get_or_create_l1_var(const Ctr& t_ctr);
    void set_penalty_in_all_sub_problems(const Var& t_var, double t_value);
    void update_penalty_parameters_independently(const std::vector<PrimalPoint>& t_primals, PenaltyUpdate& t_penalty_update);
    bool rescale_penalty_parameters(std::list<CurrentPenalty>& t_penalties);

    double fix(const Constant& t_constant, const std::vector<PrimalPoint>& t_primals);
};


#endif //IDOL_ADM_FORMULATION_H
