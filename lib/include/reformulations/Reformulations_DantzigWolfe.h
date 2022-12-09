//
// Created by henri on 09/12/22.
//

#ifndef IDOL_REFORMULATIONS_DANTZIGWOLFE_H
#define IDOL_REFORMULATIONS_DANTZIGWOLFE_H

#include "../modeling/models/Model.h"
#include "../modeling/expressions/operations/operators_Var.h"

namespace Reformulations {
    class DantzigWolfe;
}

class Reformulations::DantzigWolfe {

    Model& m_original_formulation;
    UserAttr m_is_complicating;

    Model m_restricted_master_problem;
    std::vector<Model> m_subproblems;
    std::vector<Var> m_alphas;
    std::vector<Ctr> m_convexity_constraints;
    UserAttr m_reformulated_variable;
    UserAttr m_reformulated_constraint;

    Model& model(unsigned int t_model_id);

    [[nodiscard]] unsigned int compute_number_of_subproblems() const;

    void detect_subproblem_variables();

    void throw_if_inconsistently_dispatch(const Ctr& t_ctr, const Var& t_var, unsigned int t_model_id);

    void detect_subproblem_variables(const Ctr& t_ctr, unsigned int t_model_id);

    void create_alphas();

    void create_variables();

    void create_constraints();

    void create_rmp_constraint(const Ctr& t_ctr);

    void create_subproblem_constraint(const Ctr& t_ctr, unsigned int t_model_id);

public:
    DantzigWolfe(Model& t_original_formulation, UserAttr t_is_complicating);

    [[nodiscard]] const Model& original_formulation() const { return m_original_formulation; }

    Model& restricted_master_problem() { return m_restricted_master_problem; }

    [[nodiscard]] const Model& restricted_master_problem() const { return m_restricted_master_problem; }

    auto subproblems() { return IteratorForward<std::vector<Model>>(m_subproblems); }
    [[nodiscard]] auto subproblems() const { return ConstIteratorForward<std::vector<Model>>(m_subproblems); }

    [[nodiscard]] auto alphas() const { return ConstIteratorForward<std::vector<Var>>(m_alphas); }

    [[nodiscard]] auto convexity_constraints() const { return ConstIteratorForward<std::vector<Ctr>>(m_convexity_constraints); }

    Model& subproblem(unsigned int t_index) { return m_subproblems.at(t_index - 1); }
    [[nodiscard]] const Model& subproblem(unsigned int t_index) const { return m_subproblems.at(t_index - 1); }

    [[nodiscard]] const Var& alpha(unsigned int t_index) const { return m_alphas.at(t_index-1); }

    [[nodiscard]] const Ctr& convexity_constraint(unsigned int t_index) const { return m_convexity_constraints.at(t_index-1); }

    [[nodiscard]] const UserAttr& reformulated_variable() const { return m_reformulated_variable; }

    [[nodiscard]] const UserAttr& reformulated_constraint() const { return m_reformulated_constraint; }
};

#endif //IDOL_REFORMULATIONS_DANTZIGWOLFE_H
