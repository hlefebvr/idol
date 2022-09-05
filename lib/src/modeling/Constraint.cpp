//
// Created by henri on 05/09/22.
//
#include "modeling/Constraint.h"


template<enum Player PlayerT>
Constraint<PlayerT>::Constraint(impl::Constraint<PlayerT> *t_impl) : Object<impl::Constraint<PlayerT>>(t_impl) {

}

template<enum Player PlayerT>
const Expr<PlayerT> &Constraint<PlayerT>::expr() const {
    return this->impl().expr();
}

template<enum Player PlayerT>
ConstraintType Constraint<PlayerT>::type() const {
    return this->impl().type();
}
