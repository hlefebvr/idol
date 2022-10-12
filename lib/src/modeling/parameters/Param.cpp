//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/parameters/Param.h"
#include <variant>


Param::Param(const Var &t_var) : m_impl(new Impl(t_var)) {

}

Param::Param(const Ctr &t_ctr) : m_impl(new Impl(t_ctr)) {

}

Param::~Param() {
    delete m_impl;
}

Param::Param(const Param &t_param) : m_impl(new Impl(*t_param.m_impl)) {

}

Param &Param::operator=(const Param &t_param) {
    if (this == &t_param) { return *this; }
    *m_impl = *t_param.m_impl;
    return *this;
}
