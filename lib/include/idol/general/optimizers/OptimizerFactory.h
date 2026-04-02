//
// Created by henri on 21/03/23.
//

#ifndef IDOL_OPTIMIZERFACTORY_H
#define IDOL_OPTIMIZERFACTORY_H

#include "Optimizer.h"

#include <optional>
#include <functional>

namespace idol {
    class Model;
    class Env;
    class OptimizerFactory;
    template<class CRTP>
    class OptimizerFactoryWithDefaultParameters;
}

class idol::OptimizerFactory {
protected:
    std::optional<bool> m_logs;
    std::optional<double> m_time_limit;
    std::optional<unsigned int> m_thread_limit;
    std::optional<unsigned int> m_iteration_count_limit;
    std::optional<double> m_best_bound_stop;
    std::optional<double> m_best_obj_stop;
    std::optional<bool> m_presolve;
    std::optional<bool> m_infeasible_or_unbounded_info;

    std::optional<double> m_tol_mip_relative_gap;
    std::optional<double> m_tol_mip_absolute_gap;
    std::optional<double> m_tol_integer;
    std::optional<double> m_tol_feasibility;
    std::optional<double> m_tol_optimality;

    [[nodiscard]] virtual Optimizer* create(const Model& t_model) const = 0;
public:
    virtual ~OptimizerFactory() = default;

    virtual Optimizer* operator()(const Model& t_model) const;

    [[nodiscard]] virtual OptimizerFactory* clone() const = 0;

    template<class T> T& as() {
        auto* result = dynamic_cast<T*>(this);
        if (!result) {
            throw Exception("Optimizer factory could not be cast to desired type.");
        }
        return *result;
    }

    template<class T> const T& as() const {
        auto* result = dynamic_cast<const T*>(this);
        if (!result) {
            throw Exception("Optimizer factory could not be cast to desired type.");
        }
        return *result;
    }

    template<class T> [[nodiscard]] bool is() const {
        return dynamic_cast<const T*>(this);
    }
};

template<class CRTP>
class idol::OptimizerFactoryWithDefaultParameters : public OptimizerFactory {
protected:
    CRTP& crtp() { return static_cast<CRTP&>(*this); }
    const CRTP& crtp() const { return static_cast<const CRTP&>(*this); }
public:
    CRTP& with_logs(bool t_value);
    CRTP& with_time_limit(double t_time_limit);
    CRTP& with_thread_limit(unsigned int t_max_n_threads);
    CRTP& with_iteration_limit(unsigned int t_iteration_count_limit);
    CRTP& with_best_bound_stop(double t_best_bound_stop);
    CRTP& with_best_obj_stop(double t_user_best_obj);
    CRTP& with_presolve(bool t_value);
    CRTP& with_infeasible_or_unbounded_info(bool t_value);

    CRTP& with_tol_mip_relative_gap(double t_tol_mip_relative_gap);
    CRTP& with_tol_mip_absolute_gap(double t_tol_mip_absolute_gap);
    CRTP& with_tol_integer(double t_tol_integer);
    CRTP& with_tol_feasibility(double t_tol_feasibility);
    CRTP& with_tol_optimality(double t_tol_optimality);

    CRTP& conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if);
    CRTP& conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if, const std::function<void(CRTP&)>& t_else);

};

template<class CRTP>
CRTP &
idol::OptimizerFactoryWithDefaultParameters<CRTP>::conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if, const std::function<void(CRTP&)>& t_else) {
    t_conditional_value ? t_if(crtp()) : t_else(crtp());
    return crtp();
}

template<class CRTP>
CRTP &
idol::OptimizerFactoryWithDefaultParameters<CRTP>::conditional(bool t_conditional_value, const std::function<void(CRTP&)>& t_if) {
    return conditional(t_conditional_value, t_if, [](CRTP&){});
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_infeasible_or_unbounded_info(bool t_value) {

    if (m_infeasible_or_unbounded_info.has_value()) {
        throw Exception("An infeasible-or-unbounded-info instruction has already been given.");
    }

    m_infeasible_or_unbounded_info = t_value;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_presolve(bool t_value) {

    if (m_presolve.has_value()) {
        throw Exception("A get_param_presolve instruction has already been given.");
    }

    m_presolve = t_value;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_tol_mip_absolute_gap(double t_tol_mip_absolute_gap) {

    if (m_tol_mip_absolute_gap.has_value()) {
        throw Exception("An absolute gap tolerance count limit has already been given.");
    }

    m_tol_mip_absolute_gap = t_tol_mip_absolute_gap;

    return crtp();
}

template <class CRTP>
CRTP& idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_tol_integer(double t_tol_integer) {
    if (m_tol_mip_absolute_gap.has_value()) {
        throw Exception("A tolerance for integrality has already been given.");
    }
    m_tol_integer = t_tol_integer;
    return crtp();
}

template <class CRTP>
CRTP& idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_tol_feasibility(double t_tol_feasibility) {
    if (m_tol_feasibility.has_value()) {
        throw Exception("A tolerance for feasibility has already been given.");
    }
    m_tol_feasibility = t_tol_feasibility;
    return crtp();
}

template <class CRTP>
CRTP& idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_tol_optimality(double t_tol_optimality) {

    if (m_tol_optimality.has_value()) {
        throw Exception("An optimality tolerance has already been given.");
    }

    m_tol_optimality = t_tol_optimality;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_tol_mip_relative_gap(double t_tol_mip_relative_gap) {

    if (m_tol_mip_relative_gap.has_value()) {
        throw Exception("A relative gap tolerance count limit has already been given.");
    }

    m_tol_mip_relative_gap = t_tol_mip_relative_gap;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_best_obj_stop(double t_user_best_obj) {

    if (m_best_obj_stop.has_value()) {
        throw Exception("A user best obj count limit has already been given.");
    }

    m_best_obj_stop = t_user_best_obj;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_best_bound_stop(double t_best_bound_stop) {

    if (m_best_bound_stop.has_value()) {
        throw Exception("A user best bound limit has already been given.");
    }

    m_best_bound_stop = t_best_bound_stop;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_iteration_limit(unsigned int t_iteration_count_limit) {

    if (m_iteration_count_limit.has_value()) {
        throw Exception("An iteration count limit has already been given.");
    }

    m_iteration_count_limit = t_iteration_count_limit;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_thread_limit(unsigned int t_max_n_threads) {

    if (m_thread_limit.has_value()) {
        throw Exception("A thread limit has already been given.");
    }

    m_thread_limit = t_max_n_threads;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_time_limit(double t_time_limit) {

    if (m_time_limit.has_value()) {
        throw Exception("A time limit has already been given.");
    }

    m_time_limit = t_time_limit;

    return crtp();
}

template<class CRTP>
CRTP &idol::OptimizerFactoryWithDefaultParameters<CRTP>::with_logs(bool t_value) {

    if (m_logs.has_value()) {
        throw Exception("Logging settings have already been given.");
    }

    m_logs = t_value;

    return crtp();
}

#endif //IDOL_OPTIMIZERFACTORY_H
