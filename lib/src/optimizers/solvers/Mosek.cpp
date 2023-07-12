//
// Created by henri on 27/03/23.
//
#include "optimizers/solvers/Mosek.h"
#include "optimizers/solvers/Optimizers_Mosek.h"
#include "errors/Exception.h"

idol::Optimizer *idol::Mosek::operator()(const Model &t_model) const {
#ifdef IDOL_USE_MOSEK
    auto* result = new Optimizers::Mosek(t_model, m_continuous_relaxation.has_value() && m_continuous_relaxation.value());

    this->handle_default_parameters(result);

    for (const auto& [param, value] : m_int_parameter) {
        result->set_parameter(param, value);
    }

    for (const auto& [param, value] : m_double_parameter) {
        result->set_parameter(param, value);
    }

    for (const auto& [param, value] : m_string_parameter) {
        result->set_parameter(param, value);
    }

    return result;
#else
    throw Exception("idol was not linked with Mosek.");
#endif
}

idol::Mosek idol::Mosek::ContinuousRelaxation() {
    return Mosek(true);
}

idol::Mosek *idol::Mosek::clone() const {
    return new Mosek(*this);
}

idol::Mosek &idol::Mosek::with_continuous_relaxation_only(bool t_value) {

    if (m_continuous_relaxation.has_value()) {
        throw Exception("Continuous relaxation setting has already been configured.");
    }

    m_continuous_relaxation = t_value;

    return *this;
}

idol::Mosek &idol::Mosek::with_external_parameter(const std::string &t_param, double t_value) {

    auto [it, success] = m_double_parameter.emplace(t_param, t_value);

    if (!success) {
        throw Exception("Parameter " + t_param + " has already been set.");
    }

    return *this;
}

idol::Mosek &idol::Mosek::with_external_parameter(const std::string &t_param, int t_value) {

    auto [it, success] = m_int_parameter.emplace(t_param, t_value);

    if (!success) {
        throw Exception("Parameter " + t_param + " has already been set.");
    }

    return *this;
}

idol::Mosek &idol::Mosek::with_external_parameter(const std::string &t_param, std::string t_value) {

    auto [it, success] = m_string_parameter.emplace(t_param, std::move(t_value));

    if (!success) {
        throw Exception("Parameter " + t_param + " has already been set.");
    }

    return *this;
}

