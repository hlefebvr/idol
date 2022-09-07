//
// Created by henri on 05/09/22.
//
#include "modeling/impl_Constraint.h"

template<enum Player PlayerT>
impl::Constraint<PlayerT>::Constraint(Env &t_env, unsigned int t_index, TempConstraint<PlayerT> &&t_temp_ctr, std::string &&t_name)
        : Object(t_env, t_index, std::move(t_name)), m_type(t_temp_ctr.type()), m_expression(std::move(t_temp_ctr).expr()) {


}

template<enum Player PlayerT>
Expr<PlayerT> &impl::Constraint<PlayerT>::expr() {
    return m_expression;
}

template<enum Player PlayerT>
const Expr<PlayerT> &impl::Constraint<PlayerT>::expr() const {
    return m_expression;
}

template<enum Player PlayerT>
ConstraintType impl::Constraint<PlayerT>::type() const {
    return m_type;
}

template<enum Player PlayerT>
std::string impl::Constraint<PlayerT>::default_name() const {
    return "Ctr";
}
