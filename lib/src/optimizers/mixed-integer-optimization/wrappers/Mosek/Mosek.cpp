//
// Created by henri on 27/03/23.
//
#include "idol/optimizers/mixed-integer-optimization/wrappers/Mosek/Mosek.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Mosek/Optimizers_Mosek.h"
#include "idol/errors/Exception.h"

idol::Optimizer *idol::Mosek::operator()(const Model &t_model) const {
#ifdef IDOL_USE_MOSEK
    auto* result = new Optimizers::Mosek(t_model, m_continuous_relaxation.has_value() && m_continuous_relaxation.value());

    this->handle_default_parameters(result);

    for (auto& cb : m_callbacks) {
        result->add_callback(cb->operator()());
    }

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

idol::Mosek::Mosek(const idol::Mosek & t_src)
        : m_continuous_relaxation(t_src.m_continuous_relaxation),
          m_double_parameter(t_src.m_double_parameter),
          m_int_parameter(t_src.m_int_parameter),
          m_string_parameter(t_src.m_string_parameter)
{

    for (const auto& cb : t_src.m_callbacks) {
        m_callbacks.emplace_back(cb->clone());
    }

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

idol::Mosek &idol::Mosek::add_callback(const idol::CallbackFactory &t_cb) {
    m_callbacks.emplace_back(t_cb.clone());
    return *this;
}
