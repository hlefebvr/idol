//
// Created by henri on 07/09/22.
//
#include "modeling/constraints/Constraint.h"
#include "modeling/constraints/impl_Constraint.h"

unsigned int Ctr::id() const {
    return m_impl->id();
}

const std::string &Ctr::name() const {
    return m_impl->name();
}

const Row &Ctr::row() const {
    return m_impl->row();
}
