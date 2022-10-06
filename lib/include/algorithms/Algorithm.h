//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ALGORITHM_H
#define OPTIMIZE_ALGORITHM_H

#include "modeling/solutions/Solution.h"
#include "modeling/variables/TempVar.h"
#include "modeling/constraints/TempCtr.h"
#include "errors/NotImplemented.h"
#include <list>

class AbstractNode;
class Solver;
class Var;

class AbstractAttributes;

/**
 * Solution algorithm object.
 *
 * This class represents a solution algorithm for optimization problems.
 * It is an abstract class used to share a common interface between every solution algorithm.
 * By default, most of the methods are defined and throw a NotImplemented exception.
 */
class Algorithm {
protected:
    virtual AbstractAttributes& attributes() = 0;
    [[nodiscard]] virtual const AbstractAttributes& attributes() const = 0;
public:
    virtual ~Algorithm() = default;

    /**
     * This method is typically called to initialize/set up the algorithm before solving an optimization problem.
     */
    virtual void build() {}

    /**
     * Executes the solution algorithm.
     */
    virtual void solve() = 0;

    /**
     * Searches for an IIS of the optimization problem.
     */
    virtual void compute_iis() {
        throw NotImplemented("Computing IIS", "compute_iis");
    }

    /**
     * Returns the computed primal solution (after solve has been called).
     */
    [[nodiscard]] virtual Solution::Primal primal_solution() const {
        throw NotImplemented("Retrieving primal solution", "primal_solution");
    }

    /**
     * Returns the computed dual solution (after solve has been called).
     */
    [[nodiscard]] virtual Solution::Dual dual_solution() const {
        throw NotImplemented("Retrieving dual solution", "dual_solution");
    }

    /**
     * Returns the computed farkas certificate (after solve has been called).
     */
    [[nodiscard]] virtual Solution::Dual farkas_certificate() const {
        throw NotImplemented("Retrieving Farkas certificate", "farkas_certificate");
    }

    /**
     * Returns the computed iis (after solve or solve_for_iis has been called).
     */
    [[nodiscard]] virtual Solution::Dual iis() const {
        throw NotImplemented("Retrieving IIS", "iis");
    }

    /**
     * Sets the lower bound of a variable.
     * @param t_var The queried variable.
     * @param t_lb The desired bound.
     */
    virtual void set_lower_bound(const Var& t_var, double t_lb) {
        throw NotImplemented("Updating variable LB", "set_lower_bound");
    }

    /**
     * Sets the upper bound of a variable.
     * @param t_var The queried variable.
     * @param t_lb The desired bound.
     */
    virtual void set_upper_bound(const Var& t_var, double t_ub) {
        throw NotImplemented("Updating variable UB", "set_upper_bound");
    }

    /**
     * Sets the objective function of the optimization problem.
     * @param t_objective The desired objective.
     */
    virtual void set_objective(const Row& t_objective) {
        throw NotImplemented("Updating objective", "set_objective");
    }

    /**
     * Adds a new variable to the optimization problem.
     * @param t_temporary_variable The temporary variable to create (see TempVar).
     * @return The created variable.
     */
    virtual Var add_column(TempVar t_temporary_variable) {
        throw Exception("Adding columns is not implemented. "
                               "If you wish to implement it, "
                               "please override the add_column method.");
    }

    /**
     * Removes a variable from the optimization problem.
     * @param t_variable The desired variable to be removed.
     */
    virtual void remove_variable(const Var& t_variable) {
        throw NotImplemented("Removing variable", "remove_variable");
    }

    /**
     * Sets the right handside of a constraint in the optimization problem.
     * @param t_ctr The queried constraint.
     * @param t_rhs The desired right handside.
     */
    virtual void update_constraint_rhs(const Ctr& t_ctr, double t_rhs) {
        throw NotImplemented("Updating constraint RHS", "update_constraint_rhs");
    }

    /**
     * Adds a new constraint to the optimization problem.
     * @param t_temporary_constraint The temporary constraint to create (see TempCtr).
     * @return The created constraint.
     */
    virtual Ctr add_constraint(TempCtr t_temporary_constraint) {
        throw Exception("Adding constraints is not implemented. "
                                 "If you wish to implement it, "
                                 "please override the add_constraint method.");
    }

    /**
     * Removes a constraint from the optimization problem.
     * @param t_constraint The desired constraint to be removed.
     */
    virtual void remove_constraint(const Ctr& t_constraint) {
        throw NotImplemented("Removing constraint", "remove_constraint");
    }


    /* ATTRIBUTES */

    /**
     * Sets the value of an attributes T with the value given as argument.
     * @tparam T The attribute class.
     * @param t_value The value for the attribute.
     */
    template<class T>
    void set(typename T::value_type t_value) {
        auto* ptr = dynamic_cast<typename T::attr_type*>(&attributes());
        if (ptr == nullptr) {
            throw Exception("Wrong parameter type required: " + T::name() + ".");
        }
        ptr->template set<T>(std::move(t_value));
    }

    /**
     * Returns the value of a given attribute.
     * @tparam T The attribute class.
     */
    template<class T>
    typename T::value_type get() const {
        auto* ptr = dynamic_cast<typename T::attr_type*>(&attributes());
        if (ptr == nullptr) {
            throw Exception("Wrong parameter type required: " + T::name() + ".");
        }
        return ptr->template get<T>();
    }
};

#endif //OPTIMIZE_ALGORITHM_H
