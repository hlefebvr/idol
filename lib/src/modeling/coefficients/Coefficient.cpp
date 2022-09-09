//
// Created by henri on 07/09/22.
//
#include "modeling/coefficients/Coefficient.h"
#include "modeling/numericals.h"

Coefficient Coefficient::Zero;

Coefficient::Coefficient(const Param &t_param, double t_value) : m_products({ { t_param, t_value } }) {
    if (equals(t_value, 0., ToleranceForSparsity)) {
        m_products.clear();
    }
}

Coefficient::Coefficient(double t_constant) : m_constant(t_constant) {

}

void Coefficient::set(const Param &t_param, double t_value) {

    if (equals(t_value, 0., ToleranceForSparsity)) {
        m_products.erase(t_param);
        return;
    }

    auto [it, success] = m_products.emplace(t_param, t_value);
    if (!success) {
        it->second = t_value;
    }
}

double Coefficient::get(const Param &t_param) const {
    auto it = m_products.find(t_param);
    return it == m_products.end() ? 0. : it->second;
}

Coefficient &Coefficient::operator*=(double t_factor) {

    if (equals(t_factor, 0., ToleranceForSparsity)) {
        m_constant = 0;
        m_products.clear();
        return *this;
    }

    m_constant *= t_factor;
    for (auto& [param, value] : m_products) {
        value *= t_factor;
    }

    return *this;
}

Coefficient &Coefficient::operator+=(double t_term) {
    m_constant += t_term;
    return *this;
}

Coefficient &Coefficient::operator+=(Param t_term) {
    insert_or_add(t_term, 1.);
    return *this;
}

Coefficient &Coefficient::operator+=(const Coefficient &t_term) {
    m_constant += t_term.m_constant;
    for (auto [param, value] : t_term) {
        insert_or_add(param, value);
    }
    return *this;
}

void Coefficient::insert_or_add(const Param &t_param, double t_value) {
    if (equals(t_value, 0., ToleranceForSparsity)) {
        return;
    }

    auto [it, success] = m_products.emplace(t_param, t_value);
    if (!success) {
        it->second += t_value;
        if (equals(it->second, 0., ToleranceForSparsity)) {
            m_products.erase(it);
        }
    }
}

bool Coefficient::is_zero() const {
    return m_products.empty() && equals(m_constant, 0., ToleranceForSparsity);
}

bool Coefficient::is_numerical() const {
    return m_products.empty();
}

Coefficient operator*(double t_factor, const Param& t_param) {
    if (equals(t_factor, 0., ToleranceForSparsity)) {
        return {};
    }
    return { t_param, t_factor };
}

Coefficient operator*(double t_factor, const Coefficient& t_coefficient) {
    Coefficient result(t_coefficient);
    result *= t_factor;
    return result;
}

Coefficient operator+(Coefficient t_a, const Coefficient& t_b) {
    Coefficient result(std::move(t_a));
    result += t_b;
    return result;
}