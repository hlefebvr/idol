//
// Created by henri on 05/09/22.
//
#include "modeling/impl_LinExpr.h"


template<enum Player PlayerT>
impl::LinExpr<PlayerT>::LinExpr(double t_offset) : m_constant(t_offset) {

}

template<enum Player PlayerT>
impl::LinExpr<PlayerT>::LinExpr(const ::Variable<PlayerT> &t_var) : m_terms({{t_var, 1. } }) {

}


template<enum Player PlayerT>
typename impl::LinExpr<PlayerT>::iterator impl::LinExpr<PlayerT>::begin() {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename impl::LinExpr<PlayerT>::iterator impl::LinExpr<PlayerT>::end() {
    return m_terms.end();
}

template<enum Player PlayerT>
typename impl::LinExpr<PlayerT>::const_iterator impl::LinExpr<PlayerT>::begin() const {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename impl::LinExpr<PlayerT>::const_iterator impl::LinExpr<PlayerT>::end() const {
    return m_terms.end();
}

template<enum Player PlayerT>
typename impl::LinExpr<PlayerT>::const_iterator impl::LinExpr<PlayerT>::cbegin() const {
    return m_terms.begin();
}

template<enum Player PlayerT>
typename impl::LinExpr<PlayerT>::const_iterator impl::LinExpr<PlayerT>::cend() const {
    return m_terms.end();
}

template<enum Player PlayerT>
double impl::LinExpr<PlayerT>::constant() const {
    return m_constant;
}

template<enum Player PlayerT>
unsigned int impl::LinExpr<PlayerT>::n_terms() const {
    return m_terms.size();
}

template<enum Player PlayerT>
bool impl::LinExpr<PlayerT>::is_empty() const {
    return m_terms.empty() && equals(m_constant, 0., TolFeas);
}

template<enum Player PlayerT>
impl::LinExpr<PlayerT>::LinExpr(Map<::Variable<PlayerT>, double> &&t_map, double t_constant)
        : m_terms(std::move(t_map)), m_constant(t_constant) {

}

template<enum Player PlayerT>
bool impl::LinExpr<PlayerT>::is_numerical() const {
    return m_terms.empty();
}

template<enum Player PlayerT>
double impl::LinExpr<PlayerT>::operator[](const ::Variable<PlayerT> &t_variable) const {
    auto it = m_terms.find(t_variable);
    return it == m_terms.end() ? 0. : it->second;
}


template<enum Player PlayerT>
impl::LinExpr<PlayerT>& impl::LinExpr<PlayerT>::operator*=(double t_coeff) {
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
impl::LinExpr<PlayerT>& impl::LinExpr<PlayerT>::operator+=(double t_rhs) {
    m_constant += t_rhs;
    return *this;
}

template<enum Player PlayerT>
impl::LinExpr<PlayerT>& impl::LinExpr<PlayerT>::operator+=(const impl::LinExpr<PlayerT>& t_rhs) {
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
impl::LinExpr<PlayerT>& impl::LinExpr<PlayerT>::operator+=(const ::Variable<PlayerT> &t_rhs) {
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
void impl::LinExpr<PlayerT>::set_constant(double t_constant) {
    m_constant = t_constant;
}

template<enum Player PlayerT>
void impl::LinExpr<PlayerT>::set_coefficient(const ::Variable<PlayerT> &t_var, double t_coeff) {
    if (equals(t_coeff, 0., TolFeas)) {
        m_terms.erase(t_var);
        return;
    }
    auto [it, success] = m_terms.template emplace(t_var, t_coeff);
    if (!success) {
        it->second = t_coeff;
    }
}
