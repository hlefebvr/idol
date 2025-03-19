//
// Created by henri on 18.09.24.
//

#ifndef IDOL_ADM_FORMULATION_H
#define IDOL_ADM_FORMULATION_H

#include <utility>

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/general/utils/Map.h"
#include "idol/general/utils/GenerationPattern.h"
#include <variant>

namespace idol {
    class PenaltyUpdate;
    namespace ADM {
        class Formulation;
    }
}

class idol::ADM::Formulation {
public:
    Formulation(const Model& t_src_model,
                Annotation<unsigned int> t_decomposition,
                std::optional<Annotation<double>> t_penalized_constraints,
                double t_rescaling_threshold);

    struct SubProblem {

        struct RhsFixation {
            Ctr ctr;
            QuadExpr<Var> rhs_pattern;

            RhsFixation(Ctr  t_sub_problem_ctr, QuadExpr<Var> t_pattern)
                : ctr(std::move(t_sub_problem_ctr)), rhs_pattern(std::move(t_pattern)) {}
        };

        struct RowFixation {
            std::variant<Ctr, QCtr> ctr;
            QuadExpr<Var, QuadExpr<Var>> row;

            RowFixation(const Ctr& t_sub_problem_ctr, QuadExpr<Var, QuadExpr<Var>> t_row) : ctr(t_sub_problem_ctr), row(std::move(t_row)) {}
            RowFixation(const QCtr& t_sub_problem_ctr, QuadExpr<Var, QuadExpr<Var>> t_row) : ctr(t_sub_problem_ctr), row(std::move(t_row)) {}
        };

        Model model;
        std::list<Var> l1_epigraph_vars;
        std::list<RhsFixation> rhs_fixations;
        std::list<RowFixation> row_fixations;
        QuadExpr<Var, QuadExpr<Var>> obj_fixation;

        explicit SubProblem(Env& t_env);
    };

    SubProblem& sub_problem(const Var& t_var);

    [[nodiscard]] const SubProblem& sub_problem(const Var& t_var) const;

    [[nodiscard]] unsigned int sub_problem_id(const Var& t_var) const;

    SubProblem& sub_problem(unsigned int t_sub_problem_id) { return m_sub_problems[t_sub_problem_id]; }

    [[nodiscard]] const SubProblem& sub_problem(unsigned int t_sub_problem_id) const { return m_sub_problems[t_sub_problem_id]; }

    [[nodiscard]] unsigned int n_sub_problems() const { return m_sub_problems.size(); }

    auto sub_problems() { return IteratorForward(m_sub_problems); }

    [[nodiscard]] auto sub_problems() const { return ConstIteratorForward(m_sub_problems); }

    [[nodiscard]] auto l1_epigraph_vars() const { return ConstIteratorForward(m_l1_epigraph_vars); }

    [[nodiscard]] bool has_penalized_constraints() const { return m_initial_penalty_parameters.has_value(); }

    void initialize_penalty_parameters(bool t_use_inverse_penalties);

    bool update_penalty_parameters(const std::vector<PrimalPoint>& t_primals, PenaltyUpdate& t_penalty_update); // Returns true if penalty parameters have been resacled

    void update(unsigned int t_sub_problem_id, const std::vector<PrimalPoint>& t_primals);

    struct CurrentPenalty {
        const Var variable;
        const double max_violation;
        double penalty;
        CurrentPenalty(Var t_variable, double t_max_violation, double t_penalty)
            : variable(std::move(t_variable)), max_violation(t_max_violation), penalty(t_penalty) {}
    };

private:
    Annotation<unsigned int> m_decomposition;
    std::optional<Annotation<double>> m_initial_penalty_parameters;
    double m_rescaling_threshold;

    std::vector<SubProblem> m_sub_problems;
    Map<unsigned int, Var> m_l1_epigraph_vars; // object id -> l1 epigraph variable

    [[nodiscard]] unsigned int compute_n_sub_problems(const Model& t_src_model) const;
    void initialize_sub_problems(const Model& t_src_model, unsigned int n_sub_problems);
    void dispatch_vars(const Model& t_src_model);
    void dispatch_ctrs(const Model& t_src_model);
    void dispatch_ctr(const Model& t_src_model, const Ctr& t_ctr, unsigned int t_sub_problem_id);
    void dispatch_qctrs(const Model& t_src_model);
    void dispatch_qctr(const Model& t_src_model, const QCtr& t_ctr, unsigned int t_sub_problem_id);
    void dispatch_obj(const Model& t_src_model);
    void dispatch_obj(const Model& t_src_model, unsigned int t_sub_problem_id);
    double evaluate(const QuadExpr<Var>& t_expr, const std::vector<Point<Var>>& t_primals);
    QuadExpr<Var> evaluate(const QuadExpr<Var, QuadExpr<Var>>& t_expr, const std::vector<Point<Var>>& t_primals);
    std::pair<LinExpr<Var>, AffExpr<Var>> dispatch(const LinExpr<Var>& t_expr, unsigned int t_sub_problem_id);
    void set_penalty_in_all_sub_problems(const Var& t_var, double t_value);
    bool rescale_penalty_parameters(std::list<CurrentPenalty>& t_penalties);
    LinExpr<Var> add_l1_vars(const Ctr& t_ctr, CtrType t_type, unsigned int t_sub_problem_id);
    LinExpr<Var> add_l1_vars(const QCtr& t_ctr, CtrType t_type, unsigned int t_sub_problem_id);
    LinExpr<Var> add_l1_vars(unsigned int t_ctr_id, double t_initial_penalty_parameter, CtrType t_type, unsigned int t_sub_problem_id);
};


#endif //IDOL_ADM_FORMULATION_H
