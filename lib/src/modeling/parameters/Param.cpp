//
// Created by henri on 07/09/22.
//
#include "modeling/parameters/Param.h"
#include "modeling/parameters/impl_Param.h"

Param::Param(const Var& t_var) : m_variable(t_var) {

}

unsigned int Param::id() const {
    return m_variable.id();
}

const std::string &Param::name() const {
    return m_variable.name();
}

const Var &Param::variable() const {
    return m_variable;
}
