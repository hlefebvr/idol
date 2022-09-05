//
// Created by henri on 05/09/22.
//
#include "modeling/LinExpr.h"


template<enum Player PlayerT>
const LinExpr<PlayerT> LinExpr<PlayerT>::Zero = LinExpr<PlayerT>(0.);

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(double t_offset) : m_constant(t_offset) {

}

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(const Variable<PlayerT> &t_var) : m_terms({{t_var, 1. } }) {

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
