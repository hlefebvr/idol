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

/**
 * Computes the Dantzig-Wolfe reformulation of a monolithic MI(N)LP.
 *
 * The complicating constraints are indicated by a user attribute, referred after as the subproblem flag,
 * of type `unsigned int` with the following convention:
 *
 * - n = 0: the constraint is not viewed as complicating and remains in the restricted master problem ;
 * - n > 0: the constraint is viewed as complicating and will be part of the nth subproblem.
 *
 * Note that only constraints need to have a value for the subproblem flag.
 * The variables of each subproblem are automatically detected and the subproblem flag is automatically defined on the
 * original formulation model (i.e., accessible by `Model::get<unsigned int>(const UserAttr&, const Var&)`).
 * In addition, note that if, for some variable,
 * the subproblem flag is strictly positive and does not agree with the corresponding subproblem (i.e., if
 * a variable's subproblem flag is inconsistent with a block decomposition) an exception is thrown.
 */
class Reformulations::DantzigWolfe {

    Model& m_original_formulation;
    UserAttr m_is_complicating;

    Model m_restricted_master_problem;
    std::vector<Model> m_subproblems;
    std::vector<Var> m_alphas;
    std::vector<Ctr> m_convexity_constraints;
    std::vector<UserAttr> m_original_variable;
    std::vector<UserAttr> m_original_constraint;
    UserAttr m_reformulated_variable;
    UserAttr m_reformulated_constraint;

    /**
     * Returns the nth model of the reformulation.
     * @param t_model_id The model id (i.e., n)
     * @return If n = 0, the RMP is returned, otherwise, the nth subproblem is returned.
     */
    Model& model(unsigned int t_model_id);

    /**
     * Computes the number of subproblems in the reformulation
     * @return the number of subproblems in the reformulation
     */
    [[nodiscard]] unsigned int compute_number_of_subproblems() const;

    /**
     * Automatically sets the subproblem flag of each variable.
     * Throws an exception if the flag is inconsistent.
     */
    void detect_subproblem_variables();

    /**
     * Throws a descriptive exception if the variable t_var, which was found in the constraint t_ctr,
     * does not belong to the t_model_id-th model (0 for RMP, > 0 for subproblems).
     * @param t_ctr The constraint where the variable was found
     * @param t_var The variable
     * @param t_model_id The model id
     */
    void throw_if_inconsistently_dispatched(const Ctr& t_ctr, const Var& t_var, unsigned int t_model_id);

    /**
     * Automatically sets the subproblem flag of each variable appearing in a given constraint.
     * Throws an exception if the flag is inconsistent.
     * @param t_ctr The constraint
     * @param t_model_id The model id
     */
    void detect_subproblem_variables(const Ctr& t_ctr, unsigned int t_model_id);

    /**
     * Create the alpha variables associated to each subproblem to the RMP.
     */
    void create_alphas();

    /**
     * Create the reformulated variables in the RMP and subproblems.
     * Sets the user attribute returned by Reformulations::DantzigWolfe::reformulated_variable accordingly.
     */
    void create_variables();

    void create_original_space_user_attributes();

    /**
     * Create the reformulated constraints in the RMP and subproblems.
     * Sets the user attribute returned by Reformulations::DantzigWolfe::reformulated_constraint accordingly.
     */
    void create_constraints();

    /**
     * Creates a reformulated constraint in the RMP.
     * @param t_ctr The constraint to reformulate.
     */
    void create_rmp_constraint(const Ctr& t_ctr);

    /**
     * Creates a reformulated constraint in a subproblem.
     * @param t_ctr The constraint to reformulate
     * @param t_model_id The subproblem id
     */
    void create_subproblem_constraint(const Ctr& t_ctr, unsigned int t_model_id);

public:
    /**
     * Creates and computes the Dantzig-Wolfe reformulation associated to the given original formulation based on
     * the user attribute t_is_complicating.
     * @param t_original_formulation The original formulation model
     * @param t_is_complicating The user attribute of type `unsigned int` (i.e., the subproblem flag)
     */
    DantzigWolfe(Model& t_original_formulation, UserAttr t_is_complicating);

    /**
     * Returns the model containing the original formulation
     * @return The model containing the original formulation
     */
    [[nodiscard]] const Model& original_formulation() const { return m_original_formulation; }

    unsigned int problem_id(const Var& t_var) const { return m_original_formulation.get<unsigned int>(m_is_complicating, t_var); }
    unsigned int problem_id(const Ctr& t_ctr) const { return m_original_formulation.get<unsigned int>(m_is_complicating, t_ctr); }

    /**
     * Returns the model for the RMP
     * @return The model for the RMP
     */
    Model& master_problem() { return m_restricted_master_problem; }

    /**
     * Returns the model for the RMP
     * @return The model for the RMP
     */
    [[nodiscard]] const Model& master_problem() const { return m_restricted_master_problem; }

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
    [[nodiscard]] auto alphas() const { return ConstIteratorForward<std::vector<Var>>(m_alphas); }

    /**
     * Returns an indirection to iterators to the subproblems' convexity constraint
     * @return An indirection to iterators to the subproblems' convexity constraint
     */
    [[nodiscard]] auto convexity_constraints() const { return ConstIteratorForward<std::vector<Ctr>>(m_convexity_constraints); }

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
    [[nodiscard]] const Var& alpha(unsigned int t_index) const { return m_alphas.at(t_index-1); }

    /**
     * Returns the convexity constraint associated to the nth subproblem
     * @param t_index The subproblem id (i.e., n > 0)
     * @return The nth convexity constraint
     */
    [[nodiscard]] const Ctr& convexity_constraint(unsigned int t_index) const { return m_convexity_constraints.at(t_index-1); }

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

    const UserAttr& original_space_variable(unsigned int t_index) const { return m_original_variable[t_index - 1]; }

    const UserAttr& original_space_constraint(unsigned int t_index) const { return m_original_constraint[t_index - 1]; }
};

#endif //IDOL_REFORMULATIONS_DANTZIGWOLFE_H
