//
// Created by henri on 07/09/22.
//
#include "modeling/parameters/Param.h"
#include "modeling/parameters/impl_Param.h"

unsigned int Param::id() const {
    return m_impl->id();
}

const std::string &Param::name() const {
    return m_impl->name();
}

double Param::lb() const {
    return m_impl->lb();
}

double Param::ub() const {
    return m_impl->ub();
}

VarType Param::type() const {
    return m_impl->type();
}

unsigned int Param::index() const {
    return m_impl->index();
}
