//
// Created by henri on 05/09/22.
//
#include "modeling/Expr.h"


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
const LinExpr<opp_player_v<PlayerT>> &Expr<PlayerT>::operator[](const Variable<PlayerT> &t_variable) const {
    auto it = m_terms.find(t_variable);
    return it == m_terms.end() ? LinExpr<opp_player_v<PlayerT>>::Zero : it->second;
}

template<enum Player PlayerT>
bool Expr<PlayerT>::is_numerical() const {
    return m_constant.is_numerical() && m_terms.empty();
}

template<enum Player PlayerT>
Expr<PlayerT>::Expr(LinExpr<opp_player_v<PlayerT>> t_lin_expr) : m_constant(std::move(t_lin_expr)) {

}

template<enum Player PlayerT>
Expr<PlayerT>::Expr(double t_constant) : m_constant(t_constant) {

}

template<enum Player PlayerT>
Expr<PlayerT>::Expr(const Variable<PlayerT> &t_variable) : m_terms({ { t_variable, 1. } }) {

}

template<enum Player PlayerT>
Expr<PlayerT>::Expr(const Variable<opp_player_v<PlayerT>> &t_variable) : m_constant(t_variable) {

}


template<enum Player PlayerT>
Expr<PlayerT> &Expr<PlayerT>::operator*=(double t_rhs) {
    m_constant *= t_rhs;
    for (auto& [var, coeff] : m_terms) {
        coeff *= t_rhs;
    }
    return *this;
}

template<enum Player PlayerT>
Expr<PlayerT> &Expr<PlayerT>::operator+=(const Expr<PlayerT> &t_rhs) {
    m_constant += t_rhs.m_constant;

    for (auto [var, coeff] : t_rhs) {
        auto [it, success] = m_terms.template emplace(var, coeff);
        if (!success) {
            it->second = std::move(it->second) + coeff;
            if (it->second.is_empty()) {
                m_terms.erase(it);
            }
        }
    }
    return *this;
}

template<enum Player PlayerT>
Expr<PlayerT> &Expr<PlayerT>::operator+=(double t_rhs) {
    m_constant += t_rhs;
    return *this;
}

template<enum Player PlayerT>
Expr<PlayerT> &Expr<PlayerT>::operator+=(const Variable<PlayerT> &t_rhs) {
    auto [it, success] = m_terms.template emplace(t_rhs, 1.);
    if (!success) {
        it->second += 1.;
        if (it->second.is_empty()) {
            m_terms.erase(it);
        }
    }
    return *this;
}

template<enum Player PlayerT>
Expr<PlayerT> &Expr<PlayerT>::operator+=(const Variable<opp_player_v<PlayerT>> &t_rhs) {
    m_constant += t_rhs;
    return *this;
}

template<enum Player PlayerT>
Expr<PlayerT> &Expr<PlayerT>::operator+=(const LinExpr<opp_player_v<PlayerT>> &t_rhs) {
    m_constant += t_rhs;
    return *this;
}

template<enum Player PlayerT>
void Expr<PlayerT>::set_numerical_constant(double t_constant) {
    m_constant = t_constant;
}

template<enum Player PlayerT>
void Expr<PlayerT>::set_coefficient(const Variable<PlayerT>& t_var, double t_coeff) {
    auto [it, success] = m_terms.template emplace(t_var, t_coeff);
    if (!success) {
        it->second.set_constant(t_coeff);
    }
    if (it->second.is_empty()) {
        m_terms.erase(it);
    }
}

template<enum Player PlayerT>
void Expr<PlayerT>::set_coefficient(const Variable<opp_player_v<PlayerT>>& t_var, double t_coeff) {
    m_constant.set_coefficient(t_var, t_coeff);
}

template<enum Player PlayerT>
void Expr<PlayerT>::set_coefficient(const Variable<PlayerT>& t_var, const Variable<opp_player_v<PlayerT>>& t_param, double t_coeff) {
    auto [it, success] = m_terms.template emplace(t_var, t_coeff * t_param);
    if (!success) {
        it->second.set_coefficient(t_param, t_coeff);
    }
    if (it->second.is_empty()) {
        m_terms.erase(it);
    }
}

template<enum Player PlayerT>
void Expr<PlayerT>::set_exact_coefficient(const Variable<PlayerT>& t_var, LinExpr<opp_player_v<PlayerT>> t_coefficient) {
    if (t_coefficient.is_empty()) {
        m_terms.erase(t_var);
        return;
    }
    auto it = m_terms.find(t_var);
    if (it == m_terms.end()) {
        m_terms.template emplace(t_var, std::move(t_coefficient));
    } else {
        it->second = std::move(t_coefficient);
    }
}

template<enum Player PlayerT>
void Expr<PlayerT>::set_exact_constant(LinExpr<opp_player_v<PlayerT>> t_coefficient) {
    m_constant = std::move(t_coefficient);
}
