//
// Created by henri on 21/03/23.
//

#ifndef IDOL_OPTIMIZERFACTORY_H
#define IDOL_OPTIMIZERFACTORY_H

#include "Logger.h"
#include "optimizers/Optimizer.h"

#include <optional>
#include <functional>

class Model;

/**
 * OptimizerFactory is the base class for all optimizer factories.
 *
 * An optimizer Factory is used to create an optimizer when actually needed. They can be seen as customizable
 * "building plans" for actually creating optimizers.
 */
class OptimizerFactory {
public:
    virtual ~OptimizerFactory() = default;

    /**
     * Creates and returns a new optimizer to solve the model given as parameter.
     * @param t_model The model which the optimizer will solve
     * @return A new optimizer for the model
     */
    virtual Optimizer* operator()(const Model& t_model) const = 0;

    /**
     * Creates and return a copy of the optimizer factory. This is used for polymorphism.
     * @return A copied object of the current object (i.e., *this)
     */
    [[nodiscard]] virtual OptimizerFactory* clone() const = 0;
};

template<class CRTP>
class OptimizerFactoryWithDefaultParameters : public OptimizerFactory {
    std::optional<LogLevel> m_log_level;
    std::optional<Color> m_log_color;
    std::optional<double> m_time_limit;
    std::optional<unsigned int> m_thread_limit;
    std::optional<unsigned int> m_iteration_count_limit;
    std::optional<double> m_best_bound_stop;
    std::optional<double> m_best_obj_stop;
    std::optional<double> m_relative_gap_tolerance;
    std::optional<double> m_absolute_gap_tolerance;
    std::optional<bool> m_presolve;
    std::optional<bool> m_infeasible_or_unbounded_info;
protected:
    CRTP& crtp() { return static_cast<CRTP&>(*this); }
    const CRTP& crtp() const { return static_cast<const CRTP&>(*this); }

    void handle_default_parameters(Optimizer* t_optimizer) const;
public:
    /**
     * Configures the log level and color for the optimizer
     * @param t_log_level the desired log level
     * @param t_log_color the desired output color
     * @return the optimizer factory itself
     */
    CRTP& with_log_level(LogLevel t_log_level, Color t_log_color = Color::Default);

    /**
     * Configures the time limit for the optimizer
     * @param t_time_limit the desired time limit
     * @return the optimizer factory itself
     */
    CRTP& with_time_limit(double t_time_limit);

    /**
     * Configures the maximum number of threads which the optimizer can use
     * @param t_max_n_threads the desired number of threads which can be used
     * @return the optimizer factory itself
     */
    CRTP& with_thread_limit(unsigned int t_max_n_threads);

    /**
     * Configures the maximum number of iterations which the optimizer go through
     * @param t_iteration_count_limit the desired maximum number of iterations
     * @return the optimizer factory itself
     */
    CRTP& with_iteration_count_limit(unsigned int t_iteration_count_limit);

    /**
     * Configures a threshold on the best bound for stopping the optimizer. When the optimizer have found a best bound
     * which is greater than this threshold, the optimizer stops.
     * @param t_best_bound_stop the desired threshold
     * @return the optimizer factory itself
     */
    CRTP& with_best_bound_stop(double t_best_bound_stop);

    /**
     * Configures a threshold on the best objective value for stopping the optimizer.
     * When the optimizer have found a best objective value which is less than this threshold, the optimizer stops.
     * @param t_user_best_obj the desired threshold
     * @return the optimizer factory itself
     */
    CRTP& with_best_obj_stop(double t_user_best_obj);

    /**
     * Configures the relative gap tolerance for the optimizer. When the optimizer proves that the relative optimality gap
     * is less than this threshold, the optimizer stops.
     * @param t_relative_gap_tolerance the desired relative gap tolerance
     * @return the optimizer factory itself
     */
    CRTP& with_relative_gap_tolerance(double t_relative_gap_tolerance);

    /**
     * Configures the absolute gap tolerance for the optimizer. When the optimizer proves that the absolute optimality gap
     * is less than this threshold, the optimizer stops.
     * @param t_absolute_gap_tolerance the desired absolute gap tolerance
     * @return the optimizer factory itself
     */
    CRTP& with_absolute_gap_tolerance(double t_absolute_gap_tolerance);

    /**
     * Configures the presolve activation for the optimizer.
     * @param t_value the activation level for the optimizer's presolve (0 for disabling, 1 for enabling)
     * @return the optimizer factory itself
     */
    CRTP& with_presolve(bool t_value);

    /**
     * Configures the behaviour of the optimizer when a model is shown to be infeasible or unbounded. When set to true,
     * the optimizer is forced to prove feasibility or unboundedness by providing a certificate.
     * @param t_value the activation level
     * @return the optimizer factory itself
     */
    CRTP& with_infeasible_or_unbounded_info(bool t_value);

    /**
     * Executes the lambda function given as second parameter if and only if its first argument is true. This function
     * can be used to build different optimizer factories depending on some external variable.
     * @param t_conditional_value if true, the t_if lambda function is executed, if false, nothing happens.
     * @param t_if lambda function to execute in case t_conditional_value is true
     * @return the optimizer factory itself
     */
    CRTP& conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if);

    /**
     * Executes the lambda function given as second parameter if and only if its first argument is true. This function
     * can be used to build different optimizer factories depending on some external variable.
     * @param t_conditional_value if true, the t_if lambda function is executed, if false, the t_else lambda function is.
     * @param t_if lambda function to execute in case t_conditional_value is true
     * @param t_else lambda function to execute in case t_conditional_value is false
     * @return the optimizer factory itself
     */
    CRTP& conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if, const std::function<void(CRTP&)>& t_else);

};

template<class CRTP>
CRTP &
OptimizerFactoryWithDefaultParameters<CRTP>::conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if, const std::function<void(CRTP&)>& t_else) {
    t_conditional_value ? t_if(crtp()) : t_else(crtp());
    return crtp();
}

template<class CRTP>
CRTP &
OptimizerFactoryWithDefaultParameters<CRTP>::conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if) {
    return conditional(t_conditional_value, t_if, [](CRTP&){});
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_infeasible_or_unbounded_info(bool t_value) {

    if (m_infeasible_or_unbounded_info.has_value()) {
        throw Exception("An infeasible-or-unbounded-info instruction has already been given.");
    }

    m_infeasible_or_unbounded_info = t_value;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_presolve(bool t_value) {

    if (m_presolve.has_value()) {
        throw Exception("A presolve instruction has already been given.");
    }

    m_presolve = t_value;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_absolute_gap_tolerance(double t_absolute_gap_tolerance) {

    if (m_absolute_gap_tolerance.has_value()) {
        throw Exception("An absolute gap tolerance count limit has already been given.");
    }

    m_absolute_gap_tolerance = t_absolute_gap_tolerance;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_relative_gap_tolerance(double t_relative_gap_tolerance) {

    if (m_relative_gap_tolerance.has_value()) {
        throw Exception("A relative gap tolerance count limit has already been given.");
    }

    m_relative_gap_tolerance = t_relative_gap_tolerance;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_best_obj_stop(double t_user_best_obj) {

    if (m_best_obj_stop.has_value()) {
        throw Exception("A user best obj count limit has already been given.");
    }

    m_best_obj_stop = t_user_best_obj;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_best_bound_stop(double t_user_best_bound) {

    if (m_best_bound_stop.has_value()) {
        throw Exception("A user best bound limit has already been given.");
    }

    m_best_bound_stop = t_user_best_bound;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_iteration_count_limit(unsigned int t_iteration_count_limit) {

    if (m_iteration_count_limit.has_value()) {
        throw Exception("An iteration count limit has already been given.");
    }

    m_iteration_count_limit = t_iteration_count_limit;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_thread_limit(unsigned int t_max_n_threads) {

    if (m_thread_limit.has_value()) {
        throw Exception("A thread limit has already been given.");
    }

    m_thread_limit = t_max_n_threads;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_time_limit(double t_time_limit) {

    if (m_time_limit.has_value()) {
        throw Exception("A time limit has already been given.");
    }

    m_time_limit = t_time_limit;

    return crtp();
}

template<class CRTP>
CRTP &OptimizerFactoryWithDefaultParameters<CRTP>::with_log_level(LogLevel t_log_level, Color t_log_color) {

    if (m_log_color.has_value() || m_log_level.has_value()) {
        throw Exception("Logging settings have already been given.");
    }

    m_log_level = t_log_level;
    m_log_color = t_log_color;

    return crtp();
}

template<class CRTP>
void OptimizerFactoryWithDefaultParameters<CRTP>::handle_default_parameters(Optimizer *t_optimizer) const {

    if (m_log_level.has_value()) {
        t_optimizer->set_log_level(m_log_level.value());
    }

    if (m_log_color.has_value()) {
        t_optimizer->set_log_color(m_log_color.value());
    }

    if (m_time_limit.has_value()) {
        t_optimizer->set_time_limit(m_time_limit.value());
    }

    if (m_thread_limit.has_value()) {
        t_optimizer->set_thread_limit(m_thread_limit.value());
    }

    if (m_best_bound_stop.has_value()) {
        t_optimizer->set_best_bound_stop(m_best_bound_stop.value());
    }

    if (m_best_obj_stop.has_value()) {
        t_optimizer->set_best_obj_stop(m_best_obj_stop.value());
    }

    if (m_relative_gap_tolerance.has_value()) {
        t_optimizer->set_relative_gap_tolerance(m_relative_gap_tolerance.value());
    }

    if (m_absolute_gap_tolerance.has_value()) {
        t_optimizer->set_absolute_gap_tolerance(m_absolute_gap_tolerance.value());
    }

    if (m_iteration_count_limit.has_value()) {
        t_optimizer->set_iteration_count_limit(m_iteration_count_limit.value());
    }

    if (m_presolve.has_value()) {
        t_optimizer->set_presolve(m_presolve.value());
    }

    if (m_infeasible_or_unbounded_info.has_value()) {
        t_optimizer->set_infeasible_or_unbounded_info(m_infeasible_or_unbounded_info.value());
    }

}

#endif //IDOL_OPTIMIZERFACTORY_H
