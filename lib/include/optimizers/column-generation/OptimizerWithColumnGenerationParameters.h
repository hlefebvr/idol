//
// Created by henri on 29/03/23.
//

#ifndef IDOL_OPTIMIZERWITHCOLUMNGENERATIONPARAMETERS_H
#define IDOL_OPTIMIZERWITHCOLUMNGENERATIONPARAMETERS_H

#include "optimizers/OptimizerFactory.h"

#include <optional>

namespace impl {
    template<class CRTP> class OptimizerWithColumnGenerationParameters;
}

template<class CRTP>
class impl::OptimizerWithColumnGenerationParameters : public OptimizerFactoryWithDefaultParameters<CRTP> {
protected:
    // Multithreading parameters
    std::optional<unsigned int> m_parallel_pricing_limit;

    // Column pool parameters
    std::optional<double> m_clean_up_ratio;
    std::optional<double> m_clean_up_threshold;

    // Infeasible master parameters
    std::optional<bool> m_use_farkas_pricing;
    std::optional<double> m_artificial_variables_cost;

    // Stabilization parameters
    std::optional<double> m_smoothing_factor;

    // Logging parameters
    std::optional<unsigned int> m_log_frequency;
public:
    CRTP& with_parallel_pricing_limit(unsigned int t_limit);

    CRTP& with_column_pool_clean_up(unsigned int t_threshold, double t_ratio);

    CRTP& with_farkas_pricing();

    CRTP& with_artificial_variables(double t_artificial_variable_cost);

    CRTP& with_dual_pricing_smoothing_stabilization(double t_smoothing_factor);

    CRTP& with_log_frequency(unsigned int t_log_frequency);
};

template<class CRTP>
CRTP &impl::OptimizerWithColumnGenerationParameters<CRTP>::with_log_frequency(unsigned int t_log_frequency) {

    if (m_log_frequency.has_value()) {
        throw Exception("A log frequency has already been set.");
    }

    m_log_frequency = t_log_frequency;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerWithColumnGenerationParameters<CRTP>::with_dual_pricing_smoothing_stabilization(double t_smoothing_factor) {

    if (m_smoothing_factor.has_value()) {
        throw Exception("Dual price smoothing has already been turned on.");
    }

    m_smoothing_factor = t_smoothing_factor;

    return this->crtp();
}

template<class CRTP>
CRTP &
impl::OptimizerWithColumnGenerationParameters<CRTP>::with_artificial_variables(double t_artificial_variable_cost) {

    if (m_use_farkas_pricing.has_value()) {
        throw Exception("Artificial variables is incompatible with Farkas pricing.");
    }

    if (m_artificial_variables_cost.has_value()) {
        throw Exception("Artificial variable settings has already been given.");
    }

    m_artificial_variables_cost = t_artificial_variable_cost;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerWithColumnGenerationParameters<CRTP>::with_farkas_pricing() {

    if (m_use_farkas_pricing.has_value()) {
        throw Exception("Farkas pricing has already been turned on.");
    }

    if (m_artificial_variables_cost.has_value()) {
        throw Exception("Farkas pricing is incompatible with artificial variables.");
    }

    m_use_farkas_pricing = true;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerWithColumnGenerationParameters<CRTP>::with_column_pool_clean_up(unsigned int t_threshold, double t_ratio) {

    if (m_clean_up_ratio.has_value() || m_clean_up_threshold.has_value()) {
        throw Exception("Column pool clean up settings have already been given.");
    }

    m_clean_up_threshold = t_threshold;
    m_clean_up_ratio = t_ratio;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerWithColumnGenerationParameters<CRTP>::with_parallel_pricing_limit(unsigned int t_limit) {

    if (m_parallel_pricing_limit.has_value()) {
        throw Exception("A limit for parallel pricing has already been given.");
    }

    m_parallel_pricing_limit = t_limit;

    return this->crtp();
}

#endif //IDOL_OPTIMIZERWITHCOLUMNGENERATIONPARAMETERS_H
