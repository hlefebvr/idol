//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_LINEXPR_H
#define OPTIMIZE_LINEXPR_H

#include "Variable.h"
#include "../containers/Map.h"

template<enum Player PlayerT = Decision>
class LinExpr {
    Map<Variable<PlayerT>, double> m_terms;
    double m_constant = 0.;

    explicit LinExpr(Map<Variable<PlayerT>, double>&& t_map, double t_constant);
public:
    LinExpr()= default;
    LinExpr(double t_offset); // NOLINT(google-explicit-constructor)
    LinExpr(const Variable<PlayerT>& t_var); // NOLINT(google-explicit-constructor)

    LinExpr(const LinExpr&) = default;
    LinExpr(LinExpr&&) noexcept = default;

    LinExpr<PlayerT>& operator=(const LinExpr<PlayerT>&) = default;
    LinExpr<PlayerT>& operator=(LinExpr<PlayerT>&&) noexcept = default;

    using iterator = typename Map<Variable<PlayerT>, double>::iterator;
    using const_iterator = typename Map<Variable<PlayerT>, double>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    [[nodiscard]] double constant() const;

    [[nodiscard]] unsigned int n_terms() const;

    [[nodiscard]] bool is_empty() const;

    [[nodiscard]] bool is_numerical() const;

    [[nodiscard]] double operator[](const Variable<PlayerT>& t_variable) const;

    void set_coefficient(const Variable<PlayerT>& t_var, double t_coeff);

    void set_constant(double t_constant);

    LinExpr<PlayerT> operator*=(double t_rhs);
    LinExpr<PlayerT> operator+=(double t_rhs);
    LinExpr<PlayerT> operator+=(const Variable<PlayerT>& t_rhs);
    LinExpr<PlayerT> operator+=(const LinExpr<PlayerT>& t_rhs);

    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator*(double, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator*(double, LinExpr<GenPlayerT>);
    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator+(double, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator+(const Variable<GenPlayerT>&, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator+(LinExpr<GenPlayerT>, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator+(LinExpr<GenPlayerT>, double);
    template<enum Player GenPlayerT> friend LinExpr<GenPlayerT> operator+(LinExpr<GenPlayerT>, const LinExpr<GenPlayerT>&);

    static const LinExpr<PlayerT> Zero;
};

template<enum Player PlayerT>
const LinExpr<PlayerT> LinExpr<PlayerT>::Zero = LinExpr<PlayerT>(0.);

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(double t_offset) : m_constant(t_offset) {

}

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(const Variable<PlayerT> &t_var) : m_terms({{t_var, 1. } }) {

}

template<enum Player PlayerT>
LinExpr<PlayerT> operator*(double t_coeff, const Variable<PlayerT>& t_variable) {
    if (equals(t_coeff, 0., TolFeas)) {
        return LinExpr<PlayerT>();
    }
    return LinExpr<PlayerT>({ { t_variable, t_coeff } }, 0.);
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator*(const Variable<PlayerT>& t_variable, double t_coeff) {
    return t_coeff * t_variable;
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(double t_coeff, const Variable<PlayerT>& t_variable) {
    return LinExpr<PlayerT>({ { t_variable, 1. } }, t_coeff);
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(const Variable<PlayerT>& t_variable, double t_coeff) {
    return t_coeff + t_variable;
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(const Variable<PlayerT>& t_variable_1, const Variable<PlayerT>& t_variable_2) {
    if (t_variable_1.id() == t_variable_2.id()) {
        return LinExpr<PlayerT>({ { t_variable_1, 2. } }, 0.);
    }
    return LinExpr<PlayerT>({ { t_variable_1, 1. }, { t_variable_2, 1. } }, 0.);
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(LinExpr<PlayerT> t_expr, const Variable<PlayerT>& t_variable) {
    LinExpr<PlayerT> result(std::move(t_expr));
    result += t_variable;
    return result;
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(const Variable<PlayerT>& t_variable, LinExpr<PlayerT> t_expr) {
    return std::move(t_expr) + t_variable;
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(LinExpr<PlayerT> t_expr, double t_coeff) {
    LinExpr<PlayerT> result(std::move(t_expr));
    result.m_constant += t_coeff;
    return result;
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(double t_coeff, LinExpr<PlayerT> t_expr) {
    return std::move(t_expr) + t_coeff;
}

template<enum Player PlayerT>
LinExpr<PlayerT> operator+(LinExpr<PlayerT> t_expr_1, const LinExpr<PlayerT>& t_expr_2) {
    LinExpr<PlayerT> result(std::move(t_expr_1));
    result += t_expr_2;
    return result;
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::iterator LinExpr<PlayerT>::begin() {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::iterator LinExpr<PlayerT>::end() {
    return m_terms.end();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::begin() const {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::end() const {
    return m_terms.end();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::cbegin() const {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::cend() const {
    return m_terms.end();
}

template<enum Player PlayerT>
double LinExpr<PlayerT>::constant() const {
    return m_constant;
}

template<enum Player PlayerT>
unsigned int LinExpr<PlayerT>::n_terms() const {
    return m_terms.size();
}

template<enum Player PlayerT>
bool LinExpr<PlayerT>::is_empty() const {
    return m_terms.empty() && equals(m_constant, 0., TolFeas);
}

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(Map<Variable<PlayerT>, double> &&t_map, double t_constant)
    : m_terms(std::move(t_map)), m_constant(t_constant) {

}

template<enum Player PlayerT>
bool LinExpr<PlayerT>::is_numerical() const {
    return m_terms.empty();
}

template<enum Player PlayerT>
double LinExpr<PlayerT>::operator[](const Variable<PlayerT> &t_variable) const {
    auto it = m_terms.find(t_variable);
    return it == m_terms.end() ? 0. : it->second;
}

template<enum Player GenPlayerT>
LinExpr<GenPlayerT> operator*(double t_coeff, LinExpr<GenPlayerT> t_expr) {
    LinExpr<GenPlayerT> result(std::move(t_expr));
    result *= t_coeff;
    return result;
}

template<enum Player PlayerT>
LinExpr<PlayerT> LinExpr<PlayerT>::operator*=(double t_coeff) {
    if (equals(t_coeff, 0., TolFeas)) {
        m_terms.clear();
        m_constant = 0.;
        return *this;
    }
    m_constant *= t_coeff;
    for (auto& [var, coeff] : m_terms) {
        coeff *= t_coeff;
    }
    return *this;
}

template<enum Player PlayerT>
LinExpr<PlayerT> LinExpr<PlayerT>::operator+=(double t_rhs) {
    m_constant += t_rhs;
    return *this;
}

template<enum Player PlayerT>
LinExpr<PlayerT> LinExpr<PlayerT>::operator+=(const LinExpr<PlayerT>& t_rhs) {
    m_constant += t_rhs.m_constant;
    for (auto& [var, coeff] : t_rhs.m_terms) {
        auto [it, success] = m_terms.template emplace(var, coeff);
        if (!success) {
            it->second += coeff;
            if (equals(it->second, 0., TolFeas)) {
                m_terms.erase(it);
            }
        }
    }
    return *this;
}

template<enum Player PlayerT>
LinExpr<PlayerT> LinExpr<PlayerT>::operator+=(const Variable<PlayerT> &t_rhs) {
    auto [it, success] = m_terms.template emplace(t_rhs, 1.);
    if (!success) {
        it->second += 1.;
        if (equals(it->second, 0., TolFeas)) {
            m_terms.erase(it);
        }
    }
    return *this;
}

template<enum Player PlayerT>
void LinExpr<PlayerT>::set_constant(double t_constant) {
    m_constant = t_constant;
}

template<enum Player PlayerT>
void LinExpr<PlayerT>::set_coefficient(const Variable<PlayerT> &t_var, double t_coeff) {
    if (equals(t_coeff, 0., TolFeas)) {
        m_terms.erase(t_var);
        return;
    }
    auto [it, success] = m_terms.template emplace(t_var, t_coeff);
    if (!success) {
        it->second = t_coeff;
    }
}

template<enum Player GenPlayerT>
LinExpr<GenPlayerT> operator*(LinExpr<GenPlayerT> t_expr, double t_coeff) {
    return t_coeff * std::move(t_expr);
}

template<enum Player PlayerT>
std::ostream& operator<<(std::ostream& t_os, const LinExpr<PlayerT>& t_expr) {

    t_os << t_expr.constant();
    for (const auto& [ptr_to_var, coef] : t_expr) {
        t_os << " + " << coef << " " << ptr_to_var;
    }

    return t_os;
}

#endif //OPTIMIZE_LINEXPR_H
