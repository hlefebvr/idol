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
#include "parameters/Logs.h"
#include <list>

class Node;
class Var;
class EventType;

class AbstractCallback {
    friend class Algorithm;
public:
    virtual ~AbstractCallback() = default;

    virtual Algorithm& parent() = 0;

    [[nodiscard]] virtual const Algorithm& parent() const = 0;

    virtual void execute(const EventType& t_event) = 0;

    virtual void set(const Parameter<double>& t_param, double t_value) {
        throw Exception("Could not handle parameter " + t_param.name());
    }

    virtual void set(const Parameter<bool>& t_param, bool t_value) {
        throw Exception("Could not handle parameter " + t_param.name());
    }

    virtual void set(const Parameter<int>& t_param, int t_value) {
        throw Exception("Could not handle parameter " + t_param.name());
    }

    [[nodiscard]] virtual double get(const Parameter<double>& t_param) const {
        throw Exception("Could not handle parameter " + t_param.name());
    }

    [[nodiscard]] virtual bool get(const Parameter<bool>& t_param) const {
        throw Exception("Could not handle parameter " + t_param.name());
    }

    [[nodiscard]] virtual int get(const Parameter<int>& t_param) const {
        throw Exception("Could not handle parameter " + t_param.name());
    }
};

/**
 * Solution algorithm object.
 *
 * This class represents a solution algorithm for optimization problems.
 * It is an abstract class used to share a common interface between every solution algorithm.
 * By default, most of the methods are defined and throw a NotImplemented exception.
 */
class Algorithm : public AttributeManagers::Delegate {
    friend class AlgorithmInCallback;

    Timer m_timer;
    Param::Algorithm::values<double> m_params_double;
    Param::Algorithm::values<int> m_params_int;
    Param::Algorithm::values<bool> m_params_bool;

    std::unique_ptr<AbstractCallback> m_callback;

    SolutionStatus m_solution_status = Unknown;
    Reason m_reason = NotSpecified;
    bool m_is_terminated = false;
protected:
    virtual void execute() = 0;
    virtual void execute_iis() {
        throw NotImplemented("Computing IIS", "execute_iis");
    }
    void set_status(SolutionStatus t_status) { m_solution_status = t_status; }
    void set_reason(Reason t_reason) { m_reason = t_reason; }

    void call_callback(const EventType& t_event);

    [[nodiscard]] bool has_callback() const { return !!m_callback; }
public:
    void terminate() { m_is_terminated = true; }

    [[nodiscard]] bool is_terminated() const { return m_is_terminated; }

    [[nodiscard]] const Timer& time() const { return m_timer; }

    [[nodiscard]] double remaining_time(Timer::Unit t_unit = Timer::Seconds) const { return std::max(0., get(Param::Algorithm::TimeLimit) - time().count(t_unit)); }

    [[nodiscard]] SolutionStatus status() const { return m_solution_status; }

    [[nodiscard]] Reason reason() const { return m_reason; }

    [[nodiscard]] virtual int sense() const { return Minimize; }

    /**
     * Executes the solution algorithm.
     */
    void solve() {
        m_is_terminated = false;
        m_timer.start();
        execute();
        m_timer.stop();
    }

    /**
     * Searches for an IIS of the optimization problem.
     */
    void compute_iis() {
        m_is_terminated = false;
        m_timer.start();
        execute_iis();
        m_timer.stop();
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

    /* PARAMETERS AND ATTRIBUTES */
    using AttributeManagers::Delegate::set;
    using AttributeManagers::Delegate::get;

    virtual void set(const Parameter<double>& t_param, double t_value) {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            throw Exception("Could not handle parameter " + t_param.name() + ".");
        }
        m_params_double.set(t_param, t_value);
    }

    virtual void set(const Parameter<bool>& t_param, bool t_value) {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            throw Exception("Could not handle parameter " + t_param.name() + ".");
        }
        m_params_bool.set(t_param, t_value);
    }

    virtual void set(const Parameter<int>& t_param, int t_value) {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            throw Exception("Could not handle parameter " + t_param.name() + ".");
        }
        m_params_int.set(t_param, t_value);
    }

    [[nodiscard]] virtual double get(const Parameter<double>& t_param) const {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            throw Exception("Could not handle parameter " + t_param.name() + ".");
        }
        return m_params_double.get(t_param);
    }

    [[nodiscard]] virtual bool get(const Parameter<bool>& t_param) const {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            throw Exception("Could not handle parameter " + t_param.name() + ".");
        }
        return m_params_bool.get(t_param);
    }

    [[nodiscard]] virtual int get(const Parameter<int>& t_param) const {
        if (!t_param.is_in_section(Param::Sections::Algorithm)) {
            throw Exception("Could not handle parameter " + t_param.name() + ".");
        }
        return m_params_int.get(t_param);
    }

    /* CASTS */

    template<class T> bool is() { return dynamic_cast<T*>(this) != nullptr; }

    template<class T> [[nodiscard]] bool is() const { return dynamic_cast<const T*>(this) != nullptr; }

    template<class T> T& as() { return *static_cast<T*>(this); }

    template<class T> const T& as() const { return *static_cast<const T*>(this); }

    template<class CallbackT, class ...ArgsT> CallbackT& set_user_callback(ArgsT&& ...t_args) {
        auto* result = new CallbackT(std::forward<ArgsT>(t_args)...);

        auto* cast = dynamic_cast<typename CallbackT::type*>(this);
        if (!cast) {
            throw Exception("Incompatible callback given.");
        }

        result->m_parent = cast;
        m_callback.reset(result);
        return *result;
    }
};

class EmptyAlgorithm final : public Algorithm {
protected:
    void execute() override { throw Exception("Trying to execute EmptyAlgorithm."); }
public:
    template<class ...Args> explicit EmptyAlgorithm(Args&& ...t_args) {}
};

#endif //OPTIMIZE_ALGORITHM_H
