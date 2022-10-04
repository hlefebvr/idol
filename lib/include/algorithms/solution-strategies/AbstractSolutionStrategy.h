//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H
#define OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H

#include "solvers/solutions/Solution.h"
#include "modeling/variables/TempVar.h"
#include "modeling/constraints/TempCtr.h"
#include "errors/NotImplemented.h"
#include <list>

class AbstractNode;
class Solver;
class Var;

class AbstractAttributes;

class AbstractSolutionStrategy {
protected:
    virtual AbstractAttributes& parameters() = 0;
    [[nodiscard]] virtual const AbstractAttributes& parameters() const = 0;
public:
    virtual ~AbstractSolutionStrategy() = default;

    virtual void build() {}

    virtual void solve() = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;

    [[nodiscard]] virtual Solution::Dual dual_solution() const {
        throw NotImplemented("Retrieving dual solution", "dual_solution");
    }

    [[nodiscard]] virtual Solution::Dual farkas_certificate() const {
        throw NotImplemented("Retrieving Farkas certificate", "farkas_certificate");
    }

    virtual void set_lower_bound(const Var& t_var, double t_lb) {
        throw NotImplemented("Updating variable LB", "set_lower_bound");
    }

    virtual void set_upper_bound(const Var& t_var, double t_ub) {
        throw NotImplemented("Updating variable UB", "set_upper_bound");
    }

    virtual void set_objective(const Deprecated_Row& t_objective) {
        throw NotImplemented("Updating objective", "set_objective");
    }

    virtual Var add_column(TempVar t_temporary_variable) {
        throw Exception("Adding columns is not implemented. "
                               "If you wish to implement it, "
                               "please override the add_column method.");
    }

    virtual void remove_variable(const Var& t_variable) {
        throw NotImplemented("Removing variable", "remove_variable");
    }

    virtual void update_constraint_rhs(const Ctr& t_ctr, double t_rhs) {
        throw NotImplemented("Updating constraint RHS", "update_constraint_rhs");
    }

    virtual Ctr add_constraint(TempCtr t_temporary_constraint) {
        throw Exception("Adding constraints is not implemented. "
                                 "If you wish to implement it, "
                                 "please override the add_constraint method.");
    }

    virtual void remove_constraint(const Ctr& t_constraint) {
        throw NotImplemented("Removing constraint", "remove_constraint");
    }


    /* PARAMETERS */

    template<class T>
    void set(typename T::value_type t_value) {
        auto* ptr = dynamic_cast<typename T::attr_type*>(&parameters());
        if (ptr == nullptr) {
            throw Exception("Wrong parameter type required: " + T::name() + ".");
        }
        ptr->template set<T>(std::move(t_value));
    }

    template<class T>
    typename T::value_type get() const {
        auto* ptr = dynamic_cast<typename T::attr_type*>(&parameters());
        if (ptr == nullptr) {
            throw Exception("Wrong parameter type required: " + T::name() + ".");
        }
        return ptr->template get<T>();
    }
};

#endif //OPTIMIZE_ABSTRACTSOLUTIONSTRATEGY_H
