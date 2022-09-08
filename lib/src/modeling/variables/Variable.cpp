//
// Created by henri on 07/09/22.
//
#include "modeling/variables/Variable.h"
#include "modeling/variables/impl_Variable.h"

unsigned int Var::id() const {
    return m_impl->id();
}

const std::string &Var::name() const {
    return m_impl->name();
}

const Column &Var::column() const {
    return m_impl->column();
}

