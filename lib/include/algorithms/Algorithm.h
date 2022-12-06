//
// Created by henri on 14/09/22.
//

#ifndef OPTIMIZE_ALGORITHM_H
#define OPTIMIZE_ALGORITHM_H

#include "../modeling/solutions/Solution.h"
#include "../modeling/variables/TempVar.h"
#include "../modeling/constraints/TempCtr.h"
#include "../errors/NotImplemented.h"
#include "parameters/Timer.h"
#include "parameters/Parameters_Algorithm.h"
#include "modeling/models/AttributeManager_Delegate.h"
#include "parameters/Log.h"
#include <list>

class Node;
class Var;

/**
 * Solution algorithm object.
 *
 * This class represents a solution algorithm for optimization problems.
 * It is an abstract class used to share a common interface between every solution algorithm.
 * By default, most of the methods are defined and throw a NotImplemented exception.
 */
class Algorithm /* : public AttributeManagers::Delegate */ {
    friend class AlgorithmInCallback;

    Timer m_timer;
    Param::Algorithm::values<double> m_params_double;
    Param::Algorithm::values<int> m_params_int;
    Param::Algorithm::values<bool> m_params_bool;

    SolutionStatus m_solution_status;
    Reason m_reason;
protected:
    virtual void execute() = 0;
    virtual void execute_iis() {
        throw NotImplemented("Computing IIS", "execute_iis");
    }
    void set_status(SolutionStatus t_status) { m_solution_status = t_status; }
    void set_reason(Reason t_reason) { m_reason = t_reason; }


    virtual bool set_parameter_double(const Parameter<double>& t_param, double t_value) {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            return false;
        }
        m_params_double.set(t_param, t_value);
        return true;
    }

    virtual bool set_parameter_int(const Parameter<int>& t_param, int t_value) {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            return false;
        }
        m_params_int.set(t_param, t_value);
        return true;
    }

    virtual bool set_parameter_bool(const Parameter<bool>& t_param, bool t_value) {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            return false;
        }
        m_params_bool.set(t_param, t_value);
        return true;
    }

    [[nodiscard]] virtual std::optional<double> get_parameter_double(const Parameter<double>& t_param) const {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            return {};
        }
        return m_params_double.get(t_param);
    }

    [[nodiscard]] virtual std::optional<int> get_parameter_int(const Parameter<int>& t_param) const {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            return {};
        }
        return m_params_int.get(t_param);
    }

    [[nodiscard]] virtual std::optional<bool> get_parameter_bool(const Parameter<bool>& t_param) const {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            return {};
        }
        return m_params_bool.get(t_param);
    }

public:
    virtual ~Algorithm() = default;

    [[nodiscard]] const Timer& time() const { return m_timer; }

    [[nodiscard]] double remaining_time(Timer::Unit t_unit = Timer::Seconds) const { return std::max(0., get(Param::Algorithm::TimeLimit) - time().count(t_unit)); }

    [[nodiscard]] SolutionStatus status() const { return m_solution_status; }

    [[nodiscard]] Reason reason() const { return m_reason; }

    [[nodiscard]] virtual int sense() const { return Minimize; }

    /**
     * Executes the solution algorithm.
     */
    void solve() {
        m_timer.start();
        execute();
        m_timer.stop();
    }

    /**
     * Searches for an IIS of the optimization problem.
     */
    void compute_iis() {
        m_timer.start();
        execute_iis();
        m_timer.stop();
    }

    /* VARIABLES */
    [[nodiscard]] virtual bool has(const Var& t_var) const = 0;
    [[nodiscard]] virtual double get_lb(const Var& t_var) const = 0;
    [[nodiscard]] virtual double get_ub(const Var& t_var) const = 0;
    [[nodiscard]] virtual int get_type(const Var& t_var) const = 0;
    [[nodiscard]] virtual const Column& get_column(const Var& t_var) const = 0;

    /* CONSTRAINTS */
    [[nodiscard]] virtual bool has(const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual const Row& get_row(const Ctr& t_ctr) const = 0;
    [[nodiscard]] virtual int get_type(const Ctr& t_ctr) const = 0;

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
     * Returns the computed extreme ray (after solve has been called).
     */
    [[nodiscard]] virtual Solution::Primal unbounded_ray() const {
        throw NotImplemented("Retrieving extrme rays", "extreme_ray");
    }

    /**
     * Returns the computed iis (after solve or compute_iis has been called).
     */
    [[nodiscard]] virtual Solution::Dual iis() const {
        throw NotImplemented("Retrieving IIS", "iis");
    }

    /**
     * Sets the lower bound of a variable.
     * @param t_var The queried variable.
     * @param t_lb The desired bound.
     */
    virtual void update_var_lb(const Var& t_var, double t_lb) {
        throw NotImplemented("Updating variable LB", "update_var_lb");
    }

    /**
     * Sets the upper bound of a variable.
     * @param t_var The queried variable.
     * @param t_lb The desired bound.
     */
    virtual void update_var_ub(const Var& t_var, double t_ub) {
        throw NotImplemented("Updating variable UB", "update_var_ub");
    }

    /**
     * Sets the objective function of the optimization problem.
     * @param t_objective The desired objective.
     */
    virtual void update_obj(const Expr<Var, Var>& t_objective) {
        throw NotImplemented("Updating objective", "update_obj_coeff");
    }

    /**
     * Adds a new variable to the optimization problem.
     * @param t_temporary_variable The temporary variable to create (see TempVar).
     * @return The created variable.
     */
    virtual Var add_var(TempVar&& t_temporary_variable) {
        throw Exception("Adding columns is not implemented. "
                               "If you wish to implement it, "
                               "please override the add_var method.");
    }

    virtual Var add_var(const TempVar& t_temporary_variable) {
        return add_var(TempVar(t_temporary_variable));
    }

    /**
     * Removes a variable from the optimization problem.
     * @param t_variable The desired variable to be removed.
     */
    virtual void remove(const Var& t_variable) {
        throw NotImplemented("Removing variable", "remove");
    }

    /**
     * Sets the right handside of a constraint in the optimization problem.
     * @param t_ctr The queried constraint.
     * @param t_rhs The desired right handside.
     */
    virtual void update_rhs_coeff(const Ctr& t_ctr, double t_rhs) {
        throw NotImplemented("Updating constraint RHS coefficient", "update_rhs_coeff");
    }

    /**
     * Adds a new constraint to the optimization problem.
     * @param t_temporary_constraint The temporary constraint to create (see TempCtr).
     * @return The created constraint.
     */
    virtual Ctr add_ctr(TempCtr&& t_temporary_constraint) {
        throw NotImplemented("Adding a row", "add_ctr");
    }

    virtual Ctr add_ctr(const TempCtr& t_temporary_constraint) {
        return add_ctr(TempCtr(t_temporary_constraint));
    }

    /**
     * Removes a constraint from the optimization problem.
     * @param t_constraint The desired constraint to be removed.
     */
    virtual void remove(const Ctr& t_constraint) {
        throw NotImplemented("Removing constraint", "remove");
    }

    /**
     * This function is used to write data to files.
     * @param t_filename The desired destination indication
     */
    virtual void write(const std::string& t_filename) {
        throw NotImplemented("Writing to files", "write");
    }

    /* ATTRIBUTES */
    void set(const Parameter<double>& t_param, double t_value) {
        if (!set_parameter_double(t_param, t_value)) {
            idol_Log(Warn, "algorithm", "Parameter " << t_param.name() << " was ignored.")
        }
    }

    void set(const Parameter<bool>& t_param, bool t_value) {
        if (!set_parameter_bool(t_param, t_value)) {
            idol_Log(Warn, "algorithm", "Parameter " << t_param.name() << " was ignored.")
        }
    }

    void set(const Parameter<int>& t_param, int t_value) {
        if (!set_parameter_int(t_param, t_value)) {
            idol_Log(Warn, "algorithm", "Parameter " << t_param.name() << " was ignored.")
        }
    }

    double get(const Parameter<double>& t_param) const {
        if (const auto optional  = get_parameter_double(t_param) ; optional.has_value()) {
            return optional.value();
        }
        throw Exception("Could not read parameter " + t_param.name() + ".");
    }

    bool get(const Parameter<bool>& t_param) const {
        if (const auto optional  = get_parameter_bool(t_param) ; optional.has_value()) {
            return optional.value();
        }
        throw Exception("Could not read parameter " + t_param.name() + ".");
    }

    int get(const Parameter<int>& t_param) const {
        if (const auto optional  = get_parameter_int(t_param) ; optional.has_value()) {
            return optional.value();
        }
        throw Exception("Could not read parameter " + t_param.name() + ".");
    }

    /* CASTS */

    template<class T> bool is() { return dynamic_cast<T*>(this) != nullptr; }

    template<class T> [[nodiscard]] bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

    template<class T> T& as() { return *static_cast<T*>(this); }

    template<class T> const T& as() const { return *static_cast<const T*>(this); }

};

class VoidAlgorithm final : public Algorithm {
protected:
    void execute() override { throw Exception("Executing VoidAlgorithm is not allowed."); }
public:
    template<class ...Args> explicit VoidAlgorithm(Args&& ...t_args) {}

    bool has(const Var &t_var) const override {
        throw std::runtime_error("Deprecated");
    }

    double get_lb(const Var &t_var) const override {
        throw std::runtime_error("Deprecated");
    }

    double get_ub(const Var &t_var) const override {
        throw std::runtime_error("Deprecated");
    }

    int get_type(const Var &t_var) const override {
        throw std::runtime_error("Deprecated");
    }

    const Column &get_column(const Var &t_var) const override {
        throw std::runtime_error("Deprecated");
    }

    bool has(const Ctr &t_ctr) const override {
        throw std::runtime_error("Deprecated");
    }

    const Row &get_row(const Ctr &t_ctr) const override {
        throw std::runtime_error("Deprecated");
    }

    int get_type(const Ctr &t_ctr) const override {
        throw std::runtime_error("Deprecated");
    }
};

#endif //OPTIMIZE_ALGORITHM_H
