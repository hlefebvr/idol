//
// Created by henri on 23/03/23.
//

#ifndef IDOL_MOSEK_H
#define IDOL_MOSEK_H

#include "../OptimizerFactory.h"
#include "../../containers/Map.h"

class Mosek : public OptimizerFactoryWithDefaultParameters<Mosek> {
    std::optional<bool> m_continuous_relaxation;
    Map<std::string, double> m_double_parameter;
    Map<std::string, int> m_int_parameter;
    Map<std::string, std::string> m_string_parameter;

    explicit Mosek(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    Mosek(const Mosek&) = default;
public:
    Mosek() = default;
    Mosek(Mosek&&) noexcept = default;

    Mosek& operator=(const Mosek&) = delete;
    Mosek& operator=(Mosek&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static Mosek ContinuousRelaxation();

    [[nodiscard]] Mosek *clone() const override;

    Mosek& with_continuous_relaxation_only(bool t_value);

    Mosek& with_external_parameter(const std::string& t_param, double t_value);

    Mosek& with_external_parameter(const std::string& t_param, int t_value);

    Mosek& with_external_parameter(const std::string& t_param, std::string t_value);
};

#endif //IDOL_MOSEK_H
