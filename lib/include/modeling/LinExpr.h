//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_LINEXPR_H
#define OPTIMIZE_LINEXPR_H

#include "../containers/Pointers.h"
#include "impl_LinExpr.h"
#include <memory>

template<enum Player PlayerT = Decision>
class LinExpr {
    Pointer<impl::LinExpr<PlayerT>> m_impl;

    explicit LinExpr(Map<::Variable<PlayerT>, double>&& t_map, double t_constant);
public:
    LinExpr();
    LinExpr(double t_offset); // NOLINT(google-explicit-constructor)
    LinExpr(const Variable<PlayerT>& t_var); // NOLINT(google-explicit-constructor)

    LinExpr(const LinExpr& t_src) = default;
    LinExpr(LinExpr&&) noexcept = default;

    LinExpr<PlayerT>& operator=(const LinExpr<PlayerT>&) = default;
    LinExpr<PlayerT>& operator=(LinExpr<PlayerT>&&) noexcept = default;

    using iterator = typename impl::LinExpr<PlayerT>::iterator;
    using const_iterator = typename impl::LinExpr<PlayerT>::const_iterator;

    [[nodiscard]] LinExpr<PlayerT> deep_copy() const;

    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cbegin() const;
    [[nodiscard]] const_iterator cend() const;

    [[nodiscard]] double constant() const;

    [[nodiscard]] unsigned int n_terms() const;

    [[nodiscard]] bool is_empty() const;

    [[nodiscard]] bool is_numerical() const;

    [[nodiscard]] double operator[](const Variable<PlayerT>& t_variable) const;

    void set_coefficient(const Variable<PlayerT>& t_var, double t_coeff);

    void set_constant(double t_constant);

    LinExpr<PlayerT>& operator*=(double t_rhs);
    LinExpr<PlayerT>& operator+=(double t_rhs);
    LinExpr<PlayerT>& operator+=(const Variable<PlayerT>& t_rhs);
    LinExpr<PlayerT>& operator+=(const LinExpr<PlayerT>& t_rhs);

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
    result += t_coeff;
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


template<enum Player GenPlayerT>
LinExpr<GenPlayerT> operator*(double t_coeff, LinExpr<GenPlayerT> t_expr) {
    LinExpr<GenPlayerT> result(std::move(t_expr));
    result *= t_coeff;
    return result;
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

template class LinExpr<Decision>;
template class LinExpr<Parameter>;

#endif //OPTIMIZE_LINEXPR_H
