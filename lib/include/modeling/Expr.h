//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "LinExpr.h"

template<enum Player PlayerT = Decision>
class Expr {
    Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>> m_terms;
    LinExpr<opp_player_v<PlayerT>> m_constant;

    Expr(Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>>&& t_map, LinExpr<opp_player_v<PlayerT>>&& t_constant);
public:
    Expr() = default;
    Expr(double t_constant); // NOLINT(google-explicit-constructor)
    Expr(const Variable<PlayerT>& t_variable); // NOLINT(google-explicit-constructor)
    Expr(const Variable<opp_player_v<PlayerT>>& t_variable); // NOLINT(google-explicit-constructor)
    Expr(const LinExpr<PlayerT>& t_lin_expr); // NOLINT(google-explicit-constructor)
    Expr(LinExpr<opp_player_v<PlayerT>> t_lin_expr); // NOLINT(google-explicit-constructor)

    Expr(const Expr&) = default;
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr&) = default;
    Expr& operator=(Expr&&) noexcept = default;

    [[nodiscard]] Expr<PlayerT> deep_copy() const;

    using iterator = typename Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>>::iterator;
    using const_iterator = typename Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>>::const_iterator;

    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_iterator cbegin() const;
    [[nodiscard]] const_iterator cend() const;

    [[nodiscard]] const LinExpr<opp_player_v<PlayerT>>& constant() const;

    [[nodiscard]] unsigned int n_terms() const;

    [[nodiscard]] bool is_numerical() const;

    [[nodiscard]] const LinExpr<opp_player_v<PlayerT>>& operator[](const Variable<PlayerT>& t_variable) const;

    void set_numerical_constant(double t_constant);

    void set_coefficient(const Variable<PlayerT>& t_var, double t_coeff);

    void set_coefficient(const Variable<opp_player_v<PlayerT>>& t_param, double t_coeff);

    void set_coefficient(const Variable<PlayerT>& t_var, const Variable<opp_player_v<PlayerT>>& t_param, double t_coeff);

    void set_exact_coefficient(const Variable<PlayerT>& t_var, LinExpr<opp_player_v<PlayerT>> t_coefficient);

    void set_exact_constant(LinExpr<opp_player_v<PlayerT>> t_coefficient);

    Expr<PlayerT>& operator*=(double t_rhs);

    Expr<PlayerT>& operator+=(double t_rhs);

    Expr<PlayerT>& operator+=(const Variable<PlayerT>& t_rhs);

    Expr<PlayerT>& operator+=(const Variable<opp_player_v<PlayerT>>& t_rhs);

    Expr<PlayerT>& operator+=(const LinExpr<opp_player_v<PlayerT>>& t_rhs);

    Expr<PlayerT>& operator+=(const Expr<PlayerT>& t_rhs);

    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator*(LinExpr<opp_player_v<GenPlayerT>>, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, double);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(const Variable<opp_player_v<GenPlayerT>>&, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(LinExpr<opp_player_v<GenPlayerT>>, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(LinExpr<opp_player_v<GenPlayerT>>, LinExpr<GenPlayerT>);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, const Variable<opp_player_v<GenPlayerT>>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, LinExpr<GenPlayerT>);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, LinExpr<opp_player_v<GenPlayerT>>);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, const Expr<GenPlayerT>&);
};

template<enum Player PlayerT>
Expr<PlayerT> operator*(LinExpr<opp_player_v<PlayerT>> t_coeff, const Variable<PlayerT>& t_var) {
    return Expr<PlayerT>({ { t_var, std::move(t_coeff) } }, 0.);
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, double t_coeff) {
    Expr<PlayerT> result(std::move(t_expr));
    result.m_constant += t_coeff;
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(double t_coeff, Expr<PlayerT> t_expr) {
    return std::move(t_expr) + t_coeff;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, const Variable<PlayerT>& t_var) {
    Expr<PlayerT> result(std::move(t_expr));
    result += t_var;
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(const Variable<opp_player_v<PlayerT>>& t_constant, const Variable<PlayerT>& t_var) {
    return Expr<PlayerT>({ { t_var, 1. } }, t_constant);
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Variable<PlayerT> t_var, const Expr<PlayerT>& t_expr) {
    return std::move(t_expr) + t_var;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, const Variable<opp_player_v<PlayerT>>& t_var) {
    Expr<PlayerT> result(std::move(t_expr));
    result.m_constant += t_var;
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(const Variable<opp_player_v<PlayerT>>& t_var, Expr<PlayerT> t_expr) {
    return std::move(t_expr) + t_var;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, LinExpr<PlayerT> t_lin_expr) {
    Expr<PlayerT> result(std::move(t_expr));
    result += std::move(t_lin_expr);
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(LinExpr<PlayerT> t_lin_expr, Expr<PlayerT> t_expr) {
    return std::move(t_expr) + std::move(t_lin_expr);
}


template<enum Player GenPlayerT>
Expr<GenPlayerT> operator+(Expr<GenPlayerT> t_expr, LinExpr<opp_player_v<GenPlayerT>> t_lin_expr) {
    Expr<GenPlayerT> result(std::move(t_expr));
    result.m_constant += std::move(t_lin_expr);
    return result;
}

template<enum Player GenPlayerT>
Expr<GenPlayerT> operator+(LinExpr<opp_player_v<GenPlayerT>> t_lin_expr, Expr<GenPlayerT> t_expr) {
    return std::move(t_expr) + std::move(t_lin_expr);
}


template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr_1, const Expr<PlayerT>& t_expr_2) {
    Expr<PlayerT> result(std::move(t_expr_1));
    result += t_expr_2;
    return result;
}

template<enum Player GenPlayerT>
Expr<GenPlayerT> operator+(LinExpr<opp_player_v<GenPlayerT>> t_lin_expr, const Variable<GenPlayerT> &t_var) {
    Expr<GenPlayerT> result(std::move(t_lin_expr));
    auto [it, success] = result.m_terms.template emplace(t_var, 1.);
    if (!success) {
        it->second = std::move(it->second) + 1.;
    }
    return result;
}

template<enum Player GenPlayerT>
Expr<GenPlayerT> operator+(LinExpr<opp_player_v<GenPlayerT>> t_lin_expr_1, LinExpr<GenPlayerT> t_lin_expr_2) {
    Expr<GenPlayerT> result(std::move(t_lin_expr_1));
    result = std::move(result) + t_lin_expr_2;
    return result;
}

template<enum Player PlayerT>
std::ostream &operator<<(std::ostream& t_os, const Expr<PlayerT>& t_expr) {
    t_os << t_expr.constant();
    for (const auto& [var, coeff] : t_expr) {
        t_os << " + ";
        if (coeff.is_numerical()) {
            t_os << coeff.constant();
        } else {
            t_os << '(' << coeff << ')';
        }
        t_os << " " << var;
    }
    return t_os;
}

template class Expr<Decision>;
template class Expr<Parameter>;

#endif //OPTIMIZE_EXPR_H
