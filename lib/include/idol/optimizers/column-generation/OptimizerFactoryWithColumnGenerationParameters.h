//
// Created by henri on 29/03/23.
//

#ifndef IDOL_OPTIMIZERFACTORYWITHCOLUMNGENERATIONPARAMETERS_H
#define IDOL_OPTIMIZERFACTORYWITHCOLUMNGENERATIONPARAMETERS_H

#include "idol/optimizers/OptimizerFactory.h"
#include "Optimizers_ColumnGeneration.h"

#include <optional>

namespace idol::impl {
    template<class CRTP> class OptimizerFactoryWithColumnGenerationParameters;
}

template<class CRTP>
class idol::impl::OptimizerFactoryWithColumnGenerationParameters : public OptimizerFactoryWithDefaultParameters<CRTP> {
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

    // Maximum number of columns generated per pricing operation
    std::optional<unsigned int> m_max_columns_per_pricing;

    // Non-optimal pricing phase (time_limit, gap)
    std::optional<std::pair<double, double>> m_non_optimal_pricing_phase;
protected:
    void handle_column_generation_parameters(Optimizers::ColumnGeneration* t_optimizer) const;
public:
    /**
     * Configures the maximum number of pricing problems which can be solved in parallel
     * @param t_limit the desired limit
     * @return the optimizer factory itself
     */
    CRTP& with_parallel_pricing_limit(unsigned int t_limit);

    /**
     * Configures the column pool clean up
     *
     * When the number of columns in the column pool is larger than the given threshold,
     * columns are removed from the pool (and possibly from the master problem) from the oldest columns to the
     * newest. Only t_threshold * t_ratio are kept.
     * @param t_threshold the desired threshold
     * @param t_ratio the clean up ratio
     * @return the optimizer factory itself
     */
    CRTP& with_column_pool_clean_up(unsigned int t_threshold, double t_ratio);

    /**
     * Configures the activation of Farkas pricing.
     *
     * If set to true, a Farkas certificate is asked to the master problem solver so as to generate new columns to improve
     * feasibility or to show infeasibility of the problem.
     *
     * By default, it is turned off.
     * @param t_value true if Farkas pricing is desired, false otherwise
     * @return the optimizer factory itself
     */
    CRTP& with_farkas_pricing(bool t_value);

    /**
     * Configures the cost for the artificial variables added when the master problem is infeasible and
     * Farkas pricing is turned off.
     *
     * When the master problem is infeasible, artificial variables are added to the master problem with an objective
     * cost of t_artificial_variable_cost. In case this approach fails to generate columns making the master problem
     * feasible (e.g., the costs are too small), the algorithm switches to pure phase I where the constraint violations
     * are optimized.
     *
     * If t_artificial_variable_cost is less or equal than zero, pure phase I is directly applied.
     *
     * @param t_artificial_variable_cost the desired cost for the artificial variables.
     * @return the optimizer factory itself
     */
    CRTP& with_artificial_variables_cost(double t_artificial_variable_cost);

    /**
     * Configures the stabilization factor of the dual price smoothing stabilization. This value should be in [0,1).
     * Note that when this parameter is zero, no stabilization is performed.
     * @param t_smoothing_factor the desired stabilization factor
     * @return the optimizer factory itself
     */
    CRTP& with_dual_price_smoothing_stabilization(double t_smoothing_factor);

    /**
     * Configures the frequency for logging. Logging information is displayed every t_log_frequency iterations.
     * @param t_log_frequency the desired log frequency
     * @return the optimizer factory itself
     */
    CRTP& with_log_frequency(unsigned int t_log_frequency);

    /**
     * Configures the maximum number of columns which are added for each pricing operation.
     * @param t_n_columns the maximum nuber of added columns
     * @return the optimizer itself
     */
    CRTP& with_max_columns_per_pricing(unsigned int t_n_columns);

    CRTP& with_non_optimal_pricing_phase(double t_time_limit, double t_relative_gap);
};

template<class CRTP>
CRTP &idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_non_optimal_pricing_phase(double t_time_limit,
                                                                                                 double t_relative_gap) {

    if (m_non_optimal_pricing_phase.has_value()) {
        throw Exception("Non-optimal pricing phase parameters have already been provided.");
    }

    m_non_optimal_pricing_phase = { t_time_limit, t_relative_gap };

    return this->crtp();
}

template<class CRTP>
CRTP &
idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_max_columns_per_pricing(unsigned int t_n_columns) {
    m_max_columns_per_pricing = t_n_columns;
    return this->crtp();
}

template<class CRTP>
void idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::handle_column_generation_parameters(Optimizers::ColumnGeneration *t_optimizer) const {

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

    if (m_max_columns_per_pricing.has_value()) {
        t_optimizer->set_max_columns_per_pricing(m_max_columns_per_pricing.value());
    }

    if (m_non_optimal_pricing_phase.has_value()) {
        const auto [time_limit, relative_gap] = m_non_optimal_pricing_phase.value();
        t_optimizer->set_non_optimal_pricing_phase(time_limit, relative_gap);
    }

}

template<class CRTP>
CRTP &idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_log_frequency(unsigned int t_log_frequency) {

    if (m_log_frequency.has_value()) {
        throw Exception("A log frequency has already been set.");
    }

    m_log_frequency = t_log_frequency;

    return this->crtp();
}

template<class CRTP>
CRTP &idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_dual_price_smoothing_stabilization(double t_smoothing_factor) {

    if (m_smoothing_factor.has_value()) {
        throw Exception("Dual price smoothing has already been turned on.");
    }

    m_smoothing_factor = t_smoothing_factor;

    return this->crtp();
}

template<class CRTP>
CRTP &
idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_artificial_variables_cost(double t_artificial_variable_cost) {

    if (m_artificial_variables_cost.has_value()) {
        throw Exception("Artificial variables cost have already been given.");
    }

    m_artificial_variables_cost = t_artificial_variable_cost;

    return this->crtp();
}

template<class CRTP>
CRTP &idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_farkas_pricing(bool t_value) {

    if (m_use_farkas_pricing.has_value()) {
        throw Exception("Farkas pricing setting has already been given.");
    }

    m_use_farkas_pricing = t_value;

    return this->crtp();
}

template<class CRTP>
CRTP &idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_column_pool_clean_up(unsigned int t_threshold, double t_ratio) {

    if (m_clean_up_ratio.has_value() || m_clean_up_threshold.has_value()) {
        throw Exception("Column pool clean up settings have already been given.");
    }

    m_clean_up_threshold = t_threshold;
    m_clean_up_ratio = t_ratio;

    return this->crtp();
}

template<class CRTP>
CRTP &idol::impl::OptimizerFactoryWithColumnGenerationParameters<CRTP>::with_parallel_pricing_limit(unsigned int t_limit) {

    if (m_parallel_pricing_limit.has_value()) {
        throw Exception("A limit for parallel pricing has already been given.");
    }

    m_parallel_pricing_limit = t_limit;

    return this->crtp();
}

#endif //IDOL_OPTIMIZERFACTORYWITHCOLUMNGENERATIONPARAMETERS_H
