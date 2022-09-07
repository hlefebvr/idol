//
// Created by henri on 05/09/22.
//
#include "modeling/LinExpr.h"

template<enum Player PlayerT>
const LinExpr<PlayerT> LinExpr<PlayerT>::Zero = LinExpr<PlayerT>(0.);

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(Map<Variable<PlayerT>, double> &&t_map, double t_constant)
    : m_impl(new impl::LinExpr<PlayerT>(std::move(t_map), t_constant)) {

}

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr() : m_impl(new impl::LinExpr<PlayerT>()) {

}

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(double t_offset) : m_impl(new impl::LinExpr<PlayerT>(t_offset)) {

}

template<enum Player PlayerT>
LinExpr<PlayerT>::LinExpr(const Variable<PlayerT> &t_var) : m_impl(new impl::LinExpr<PlayerT>(t_var)) {

}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::iterator LinExpr<PlayerT>::begin() {
    return m_impl->begin();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::iterator LinExpr<PlayerT>::end() {
    return m_impl->end();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::begin() const {
    return m_impl->cbegin();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::end() const {
    return m_impl->cend();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::cbegin() const {
    return m_impl->cbegin();
}

template<enum Player PlayerT>
typename LinExpr<PlayerT>::const_iterator LinExpr<PlayerT>::cend() const {
    return m_impl->cend();
}

template<enum Player PlayerT>
unsigned int LinExpr<PlayerT>::n_terms() const {
    return m_impl->n_terms();
}

template<enum Player PlayerT>
double LinExpr<PlayerT>::constant() const {
    return m_impl->constant();
}

template<enum Player PlayerT>
bool LinExpr<PlayerT>::is_empty() const {
    return m_impl->is_empty();
}

template<enum Player PlayerT>
bool LinExpr<PlayerT>::is_numerical() const {
    return m_impl->is_numerical();
}

template<enum Player PlayerT>
double LinExpr<PlayerT>::operator[](const Variable<PlayerT> &t_variable) const {
    return m_impl->operator[](t_variable);
}

template<enum Player PlayerT>
void LinExpr<PlayerT>::set_coefficient(const Variable<PlayerT> &t_var, double t_coeff) {
    m_impl->set_coefficient(t_var, t_coeff);
}

template<enum Player PlayerT>
void LinExpr<PlayerT>::set_constant(double t_constant) {
    m_impl->set_constant(t_constant);
}

template<enum Player PlayerT>
LinExpr<PlayerT>& LinExpr<PlayerT>::operator*=(double t_rhs) {
    m_impl->operator*=(t_rhs);
    return *this;
}

template<enum Player PlayerT>
LinExpr<PlayerT>& LinExpr<PlayerT>::operator+=(double t_rhs) {
    m_impl->operator+=(t_rhs);
    return *this;
}

template<enum Player PlayerT>
LinExpr<PlayerT>& LinExpr<PlayerT>::operator+=(const Variable<PlayerT> &t_rhs) {
    m_impl->operator+=(t_rhs);
    return *this;
}

template<enum Player PlayerT>
LinExpr<PlayerT>& LinExpr<PlayerT>::operator+=(const LinExpr<PlayerT> &t_rhs) {
    m_impl->operator+=(*t_rhs.m_impl);
    return *this;
}
#include <iostream>
template<enum Player PlayerT>
LinExpr<PlayerT> LinExpr<PlayerT>::deep_copy() const {
    LinExpr result;
    result.m_impl = new impl::LinExpr<PlayerT>(*m_impl);
    return result;
}
