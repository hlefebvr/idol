//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/expressions/Constant.h"
#include "../../../include/modeling/numericals.h"
#include "../../../include/modeling/solutions/Solution.h"
#include <memory>

Constant Constant::Zero;

Constant::Constant(const Param &t_param, double t_value) : m_products({ { t_param, t_value } }) {
    if (equals(t_value, 0., ToleranceForSparsity)) {
        m_products.clear();
    }
}

Constant::Constant(double t_constant) : m_constant(t_constant) {

}

void Constant::set(const Param &t_param, double t_value) {

    if (equals(t_value, 0., ToleranceForSparsity)) {
        m_products.erase(t_param);
        return;
    }

    auto [it, success] = m_products.emplace(t_param, t_value);
    if (!success) {
        it->second = t_value;
    }
}

double Constant::get(const Param &t_param) const {
    auto it = m_products.find(t_param);
    return it == m_products.end() ? 0. : it->second;
}

Constant &Constant::operator*=(double t_factor) {

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

Constant &Constant::operator+=(double t_term) {
    m_constant += t_term;
    return *this;
}

Constant &Constant::operator+=(Param t_term) {
    insert_or_add(t_term, 1.);
    return *this;
}

Constant &Constant::operator+=(const Constant &t_term) {
    m_constant += t_term.m_constant;
    for (auto [param, value] : t_term) {
        insert_or_add(param, value);
    }
    return *this;
}

Constant &Constant::operator-=(double t_term) {
    m_constant -= t_term;
    return *this;
}

Constant &Constant::operator-=(Param t_term) {
    insert_or_add(t_term, -1.);
    return *this;
}

Constant &Constant::operator-=(const Constant &t_term) {
    m_constant -= t_term.m_constant;
    for (auto [param, value] : t_term) {
        insert_or_add(param, -value);
    }
    return *this;
}

void Constant::insert_or_add(const Param &t_param, double t_value) {
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

bool Constant::is_zero() const {
    return m_products.empty() && equals(m_constant, 0., ToleranceForSparsity);
}

bool Constant::is_numerical() const {
    return m_products.empty();
}

double Constant::fix(const Solution::Primal &t_primals) const {
    double result = m_constant;
    for (const auto& [param, coeff] : m_products) {
        result += coeff * t_primals.get(param.as<Var>());
    }
    return result;
}

double Constant::fix(const Solution::Dual &t_primals) const {
    double result = m_constant;
    for (const auto& [param, coeff] : m_products) {
        result += coeff * t_primals.get(param.as<Ctr>());
    }
    return result;
}
