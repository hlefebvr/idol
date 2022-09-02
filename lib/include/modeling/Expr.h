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
    Expr(const LinExpr<PlayerT>& t_lin_expr); // NOLINT(google-explicit-constructor)

    Expr(const Expr&) = default;
    Expr(Expr&&) noexcept = default;

    Expr& operator=(const Expr&) = default;
    Expr& operator=(Expr&&) noexcept = default;

    using iterator = typename Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>>::iterator;
    using const_iterator = typename Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>>::const_iterator;

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;

    [[nodiscard]] const LinExpr<opp_player_v<PlayerT>>& constant() const;

    [[nodiscard]] unsigned int n_terms() const;

    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator*(LinExpr<opp_player_v<GenPlayerT>>, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, double);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, const Variable<GenPlayerT>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, const Variable<opp_player_v<GenPlayerT>>&);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, LinExpr<GenPlayerT>);
    template<enum Player GenPlayerT> friend Expr<GenPlayerT> operator+(Expr<GenPlayerT>, Expr<GenPlayerT>);
};

template<enum Player PlayerT>
Expr<PlayerT>::Expr(const LinExpr<PlayerT> &t_lin_expr) : m_constant(t_lin_expr.constant()) {
    for (auto [var, coeff] : t_lin_expr) {
        m_terms.template emplace(var, coeff);
    }
}

template<enum Player PlayerT>
Expr<PlayerT>::Expr(Map<Variable<PlayerT>, LinExpr<opp_player_v<PlayerT>>> &&t_map,
                    LinExpr<opp_player_v<PlayerT>> &&t_constant) : m_terms(std::move(t_map)), m_constant(std::move(t_constant)){

}

template<enum Player PlayerT>
typename Expr<PlayerT>::iterator Expr<PlayerT>::begin() {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename Expr<PlayerT>::iterator Expr<PlayerT>::end() {
    return m_terms.end();
}

template<enum Player PlayerT>
typename Expr<PlayerT>::const_iterator Expr<PlayerT>::begin() const {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename Expr<PlayerT>::const_iterator Expr<PlayerT>::end() const {
    return m_terms.end();
}

template<enum Player PlayerT>
typename Expr<PlayerT>::const_iterator Expr<PlayerT>::cbegin() const {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename Expr<PlayerT>::const_iterator Expr<PlayerT>::cend() const {
    return m_terms.end();
}

template<enum Player PlayerT>
const LinExpr<opp_player_v<PlayerT>>& Expr<PlayerT>::constant() const {
    return m_constant;
}

template<enum Player PlayerT>
unsigned int Expr<PlayerT>::n_terms() const {
    return m_terms.size();
}

template<enum Player PlayerT>
Expr<PlayerT> operator*(LinExpr<opp_player_v<PlayerT>> t_coeff, const Variable<PlayerT>& t_var) {
    return Expr<PlayerT>({ { t_var, std::move(t_coeff) } }, 0.);
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, double t_coeff) {
    Expr<PlayerT> result(std::move(t_expr));
    result.m_constant = std::move(result.m_constant) + t_coeff;
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(double t_coeff, Expr<PlayerT> t_expr) {
    return std::move(t_expr) + t_coeff;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, const Variable<PlayerT>& t_var) {
    Expr<PlayerT> result(std::move(t_expr));
    auto [it, success] = result.m_terms.template emplace(t_var, 1.);
    if (!success) {
        it->second = std::move(it->second) + 1.;
    }
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Variable<PlayerT> t_var, const Expr<PlayerT>& t_expr) {
    return std::move(t_expr) + t_var;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, const Variable<opp_player_v<PlayerT>>& t_var) {
    Expr<PlayerT> result(std::move(t_expr));
    result.m_constant = std::move(result.m_constant) + t_var;
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(const Variable<opp_player_v<PlayerT>>& t_var, Expr<PlayerT> t_expr) {
    return std::move(t_expr) + t_var;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr, LinExpr<PlayerT> t_lin_expr) {
    Expr<PlayerT> result(std::move(t_expr));
    result.m_constant = std::move(result.m_constant) + t_lin_expr.constant();
    for (auto&& term : t_lin_expr) {
        auto&& [var, coeff] = term;
        auto [it, success] = result.m_terms.template emplace(var, coeff);
        if (!success) {
            it->second = std::move(it->second) + coeff;
        }
    }
    return result;
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(LinExpr<PlayerT> t_lin_expr, Expr<PlayerT> t_expr) {
    return std::move(t_expr) + std::move(t_lin_expr);
}

template<enum Player PlayerT>
Expr<PlayerT> operator+(Expr<PlayerT> t_expr_1, Expr<PlayerT> t_expr_2) {
    const bool t_expr_1_is_moved = t_expr_1.n_terms() >= t_expr_2.n_terms();

    auto&& expr_1     = t_expr_1_is_moved ? std::move(t_expr_1) : std::move(t_expr_2);
    const auto expr_2 = t_expr_1_is_moved ?      t_expr_2       :     t_expr_1       ;

    Expr<PlayerT> result(std::move(expr_1));
    result.m_constant = std::move(result.m_constant) + expr_2.m_constant;

    for (auto [var, coeff] : expr_2) {
        auto [it, success] = result.m_terms.template emplace(var, coeff);
        if (!success) {
            it->second = std::move(it->second) + coeff;
        }
    }

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

#endif //OPTIMIZE_EXPR_H
