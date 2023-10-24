//
// Created by henri on 07/09/22.
//
#include "idol/modeling/expressions/Constant.h"
#include "idol/modeling/numericals.h"
#include "idol/modeling/solutions/Solution.h"
#include <memory>

idol::Constant idol::Constant::Zero;

idol::Constant::Constant(const Param &t_param, double t_value) : m_linear_terms({{t_param, t_value } }) {
    if (equals(t_value, 0., Tolerance::Sparsity)) {
        m_linear_terms.clear();
    }
}

idol::Constant::Constant(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value)  {

    m_quadratic_terms.emplace(std::make_pair<>(t_param_1, t_param_2), t_value);

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        m_quadratic_terms.clear();
    }
}

idol::Constant::Constant(double t_constant) : m_constant(t_constant) {

}

void idol::Constant::set(const Param &t_param, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        m_linear_terms.erase(t_param);
        return;
    }

    auto [it, success] = m_linear_terms.emplace(t_param, t_value);
    if (!success) {
        it->second = t_value;
    }
}

double idol::Constant::get(const Param &t_param) const {
    auto it = m_linear_terms.find(t_param);
    return it == m_linear_terms.end() ? 0. : it->second;
}

double idol::Constant::get(const idol::Param &t_param_1, const idol::Param &t_param_2) const {
    auto it = m_quadratic_terms.find(std::make_pair(t_param_1, t_param_2));
    return it == m_quadratic_terms.end() ? 0. : it->second;
}

idol::Constant &idol::Constant::operator*=(double t_factor) {

    if (equals(t_factor, 0., Tolerance::Sparsity)) {
        m_constant = 0;
        m_linear_terms.clear();
        m_quadratic_terms.clear();
        return *this;
    }

    m_constant *= t_factor;
    for (auto& [param, value] : m_linear_terms) {
        value *= t_factor;
    }
    for (auto& [pair, value] : m_quadratic_terms) {
        value *= t_factor;
    }

    return *this;
}

idol::Constant &idol::Constant::operator+=(double t_term) {
    m_constant += t_term;
    return *this;
}

idol::Constant &idol::Constant::operator+=(const Param& t_term) {
    insert_or_add(t_term, 1.);
    return *this;
}

idol::Constant &idol::Constant::operator+=(const Constant &t_term) {
    m_constant += t_term.m_constant;
    for (auto [param, value] : t_term.linear()) {
        insert_or_add(param, value);
    }
    for (auto [pair, value] : t_term.quadratic()) {
        insert_or_add(pair.first, pair.second, value);
    }
    return *this;
}

idol::Constant &idol::Constant::operator-=(double t_term) {
    m_constant -= t_term;
    return *this;
}

idol::Constant &idol::Constant::operator-=(Param t_term) {
    insert_or_add(t_term, -1.);
    return *this;
}

idol::Constant &idol::Constant::operator-=(const Constant &t_term) {
    m_constant -= t_term.m_constant;
    for (auto [param, value] : t_term.linear()) {
        insert_or_add(param, -value);
    }
    for (auto [pair, value] : t_term.quadratic()) {
        insert_or_add(pair.first, pair.second, -value);
    }
    return *this;
}

void idol::Constant::insert_or_add(const Param &t_param, double t_value) {
    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }

    auto [it, success] = m_linear_terms.emplace(t_param, t_value);
    if (!success) {
        it->second += t_value;
        if (equals(it->second, 0., Tolerance::Sparsity)) {
            m_linear_terms.erase(it);
        }
    }
}

void idol::Constant::insert_or_add(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value) {
    if (equals(t_value, 0., Tolerance::Sparsity)) {
        return;
    }

    auto [it, success] = m_quadratic_terms.emplace(std::make_pair(t_param_1, t_param_2), t_value);
    if (!success) {
        it->second += t_value;
        if (equals(it->second, 0., Tolerance::Sparsity)) {
            m_quadratic_terms.erase(it);
        }
    }
}

bool idol::Constant::is_zero() const {
    return m_linear_terms.empty() && m_quadratic_terms.empty() && equals(m_constant, 0., Tolerance::Sparsity);
}

bool idol::Constant::is_numerical() const {
    return m_linear_terms.empty() && m_quadratic_terms.empty();
}

double idol::Constant::fix(const Solution::Primal &t_primals) const {
    double result = m_constant;
    for (const auto& [param, coeff] : m_linear_terms) {
        result += coeff * t_primals.get(param.as<Var>());
    }
    for (const auto& [pair, coeff] : m_quadratic_terms) {
        const auto [param1, param2] = pair;
        result += coeff * t_primals.get(param1.as<Var>()) * t_primals.get(param2.as<Var>());
    }
    return result;
}

double idol::Constant::fix(const Solution::Dual &t_duals) const {
    double result = m_constant;
    for (const auto& [param, coeff] : m_linear_terms) {
        result += coeff * t_duals.get(param.as<Ctr>());
    }
    for (const auto& [pair, coeff] : m_quadratic_terms) {
        const auto [param1, param2] = pair;
        result += coeff * t_duals.get(param1.as<Ctr>()) * t_duals.get(param2.as<Ctr>());
    }
    return result;
}

void idol::Constant::set(const idol::Param &t_param_1, const idol::Param &t_param_2, double t_value) {

    if (equals(t_value, 0., Tolerance::Sparsity)) {
        m_quadratic_terms.erase(std::make_pair( t_param_1, t_param_2 ));
        return;
    }

    auto [it, success] = m_quadratic_terms.emplace(std::make_pair(t_param_1, t_param_2), t_value);
    if (!success) {
        it->second = t_value;
    }
}

