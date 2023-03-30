//
// Created by henri on 29/03/23.
//

#ifndef IDOL_OPTIMIZERFACTORYWITHCOLUMNGENERATIONPARAMETERS_H
#define IDOL_OPTIMIZERFACTORYWITHCOLUMNGENERATIONPARAMETERS_H

#include "optimizers/OptimizerFactory.h"
#include "Optimizers_ColumnGeneration.h"

#include <optional>

namespace impl {
    template<class CRTP> class OptimizerFactoryWithColumnGenerationParameters;
}

template<class CRTP>
class impl::OptimizerFactoryWithColumnGenerationParameters : public OptimizerFactoryWithDefaultParameters<CRTP> {
    // Multithreading parameters
    std::optional<unsigned int> m_parallel_pricing_limit;

    // Column pool parameters
    std::optional<double> m_clean_up_ratio;
    std::optional<unsigned int> m_clean_up_threshold;

    // Infeasible master parameters
    std::optional<bool> m_use_farkas_pricing;
    std::optional<double> m_artificial_variables_cost;

    // Stabilization parameters
    std::optional<double> m_smoothing_factor;

    // Logging parameters
    std::optional<unsigned int> m_log_frequency;
protected:
    void handle_column_generation_parameters(Optimizers::ColumnGeneration* t_optimizer) const;
public:
    CRTP& with_parallel_pricing_limit(unsigned int t_limit);

    CRTP& with_column_pool_clean_up(unsigned int t_threshold, double t_ratio);

    CRTP& with_farkas_pricing(bool t_value);

    CRTP& with_artificial_variables_cost(double t_artificial_variable_cost);

    CRTP& with_dual_price_smoothing_stabilization(double t_smoothing_factor);

    CRTP& with_log_frequency(unsigned int t_log_frequency);
};

template<class CRTP>
void impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::handle_column_generation_parameters(Optimizers::ColumnGeneration *t_optimizer) const {

    if (m_parallel_pricing_limit.has_value()) {
        t_optimizer->set_parallel_pricing_limit(m_parallel_pricing_limit.value());
    }

    if (m_clean_up_ratio.has_value()) {
        t_optimizer->set_clean_up_ratio(m_clean_up_ratio.value());
    }

    if (m_clean_up_threshold.has_value()) {
        t_optimizer->set_clean_up_threshold(m_clean_up_threshold.value());
    }

    if (m_use_farkas_pricing.has_value()) {
        t_optimizer->set_farkas_pricing(m_use_farkas_pricing.value());
    }

    if (m_artificial_variables_cost.has_value()) {
        t_optimizer->set_artificial_variables_cost(m_artificial_variables_cost.value());
    }

    if (m_smoothing_factor.has_value()) {
        t_optimizer->set_dual_price_smoothing_stabilization_factor(m_smoothing_factor.value());
    }

    if (m_log_frequency.has_value()) {
        t_optimizer->set_log_frequency(m_log_frequency.value());
    }

}

template<class CRTP>
CRTP &impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_log_frequency(unsigned int t_log_frequency) {

    if (m_log_frequency.has_value()) {
        throw Exception("A log frequency has already been set.");
    }

    m_log_frequency = t_log_frequency;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_dual_price_smoothing_stabilization(double t_smoothing_factor) {

    if (m_smoothing_factor.has_value()) {
        throw Exception("Dual price smoothing has already been turned on.");
    }

    m_smoothing_factor = t_smoothing_factor;

    return this->crtp();
}

template<class CRTP>
CRTP &
impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_artificial_variables_cost(double t_artificial_variable_cost) {

    if (m_artificial_variables_cost.has_value()) {
        throw Exception("Artificial variables cost have already been given.");
    }

    m_artificial_variables_cost = t_artificial_variable_cost;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_farkas_pricing(bool t_value) {

    if (m_use_farkas_pricing.has_value()) {
        throw Exception("Farkas pricing setting has already been given.");
    }

    m_use_farkas_pricing = t_value;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_column_pool_clean_up(unsigned int t_threshold, double t_ratio) {

    if (m_clean_up_ratio.has_value() || m_clean_up_threshold.has_value()) {
        throw Exception("Column pool clean up settings have already been given.");
    }

    m_clean_up_threshold = t_threshold;
    m_clean_up_ratio = t_ratio;

    return this->crtp();
}

template<class CRTP>
CRTP &impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_parallel_pricing_limit(unsigned int t_limit) {

    if (m_parallel_pricing_limit.has_value()) {
        throw Exception("A limit for parallel pricing has already been given.");
    }

    m_parallel_pricing_limit = t_limit;

    return this->crtp();
}

#endif //IDOL_OPTIMIZERFACTORYWITHCOLUMNGENERATIONPARAMETERS_H
