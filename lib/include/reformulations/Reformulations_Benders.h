//
// Created by henri on 04/01/23.
//

#ifndef IDOL_REFORMULATIONS_BENDERS_H
#define IDOL_REFORMULATIONS_BENDERS_H

#include "modeling/models/Model.h"

namespace Reformulations {
    class Benders;
}

class Reformulations::Benders {
    Model& m_original_formulation;
    UserAttr m_subproblem_flag;

    Model m_master_problem;
    std::vector<Model> m_subproblems;
    std::vector<Var> m_thetas;
    std::vector<Ctr> m_benders_cuts;
    std::vector<UserAttr> m_original_variable;
    std::vector<UserAttr> m_original_constraint;
    UserAttr m_reformulated_variable;
    UserAttr m_reformulated_constraint;

    [[nodiscard]] unsigned int compute_number_of_subproblems() const;

    /**
     * Returns the nth model of the reformulation.
     * @param t_model_id The model id (i.e., n)
     * @return If n = 0, the RMP is returned, otherwise, the nth subproblem is returned.
     */
    Model& model(unsigned int t_model_id);

    void detect_subproblem_constraints();
    void detect_subproblem_constraints(const Var& t_var, unsigned int t_model_id);
    void throw_if_inconsistently_dispatched(const Var& t_var, const Ctr& t_ctr, unsigned int t_model_id);

    void create_original_space_user_attributes();
    void create_constraints();
    void create_variables();
    void add_row_master(const Ctr& t_created_ctr, const Row& t_original_row);
    void add_row_subproblem(const Ctr& t_created_ctr, const Row& t_original_row, Expr<Var, Var>& t_benders_cut, unsigned int t_model_id);
public:
    Benders(Model& t_original_formulation, UserAttr t_subproblem_flag);

    /**
     * Returns the model containing the original formulation
     * @return The model containing the original formulation
     */
    [[nodiscard]] const Model& original_formulation() const { return m_original_formulation; }

    [[nodiscard]] unsigned int problem_id(const Var& t_var) const { return m_original_formulation.get<unsigned int>(m_subproblem_flag, t_var); }
    [[nodiscard]] unsigned int problem_id(const Ctr& t_ctr) const { return m_original_formulation.get<unsigned int>(m_subproblem_flag, t_ctr); }

    /**
     * Returns the model for the RMP
     * @return The model for the RMP
     */
    Model& master_problem() { return m_master_problem; }

    /**
     * Returns the model for the RMP
     * @return The model for the RMP
     */
    [[nodiscard]] const Model& master_problem() const { return m_master_problem; }

    /**
     * Returns an indirection to iterators to the subproblems' model
     * @return An indirection to iterators to the subproblems' model
     */
    auto subproblems() { return IteratorForward<std::vector<Model>>(m_subproblems); }

    /**
     * Returns an indirection to iterators to the subproblems' model
     * @return An indirection to iterators to the subproblems' model
     */
    [[nodiscard]] auto subproblems() const { return ConstIteratorForward<std::vector<Model>>(m_subproblems); }

    /**
     * Returns an indirection to iterators to the subproblems' alpha variable
     * @return An indirection to iterators to the subproblems' alpha variable
     */
    [[nodiscard]] auto thetas() const { return ConstIteratorForward<std::vector<Var>>(m_thetas); }

    /**
     * Returns an indirection to iterators to the subproblems' benders' cut
     * @return An indirection to iterators to the subproblems' benders' cut
     */
    [[nodiscard]] auto benders_cuts() const { return ConstIteratorForward<std::vector<Ctr>>(m_benders_cuts); }

    /**
     * Returns the nth subproblem
     * @param t_index The subproblem id (i.e., n > 0)
     * @return The nth subproblem
     */
    Model& subproblem(unsigned int t_index) { return m_subproblems.at(t_index - 1); }

    /**
     * Returns the nth subproblem
     * @param t_index The subproblem id (i.e., n > 0)
     * @return The nth subproblem
     */
    [[nodiscard]] const Model& subproblem(unsigned int t_index) const { return m_subproblems.at(t_index - 1); }

    /**
     * Returns the alpha variable associated to the nth subproblem
     * @param t_index The subproblem id (i.e., n > 0)
     * @return The nth alpha variable
     */
    [[nodiscard]] const Var& theta(unsigned int t_index) const { return m_thetas.at(t_index-1); }

    /**
     * Returns the convexity constraint associated to the nth subproblem
     * @param t_index The subproblem id (i.e., n > 0)
     * @return The nth convexity constraint
     */
    [[nodiscard]] const Ctr& benders_cut(unsigned int t_index) const { return m_benders_cuts.at(t_index-1); }

    /**
     * Returns a user attribute of type Var for the original formulation's model containing, for each variable of the
     * original formulation, the associated reformulated variable.
     * @return The user attribute
     */
    [[nodiscard]] const UserAttr& reformulated_variable() const { return m_reformulated_variable; }

    /**
     * Returns a user attribute of type Ctr for the original formulation's model containing, for each constraint of the
     * original formulation, the associated reformulated constraint.
     * @return The user attribute
     */
    [[nodiscard]] const UserAttr& reformulated_constraint() const { return m_reformulated_constraint; }

    [[nodiscard]] const UserAttr& original_space_variable(unsigned int t_index) const { return m_original_variable[t_index - 1]; }

    [[nodiscard]] const UserAttr& original_space_constraint(unsigned int t_index) const { return m_original_constraint[t_index - 1]; }
};

#endif //IDOL_REFORMULATIONS_BENDERS_H
