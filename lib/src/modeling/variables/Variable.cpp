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

double Var::lb() const {
    return m_impl->lb();
}

double Var::ub() const {
    return m_impl->ub();
}

VarType Var::type() const {
    return m_impl->type();
}

const Coefficient& Var::obj() const {
    return m_impl->column().constant();
}

const Coefficient &Var::get(const Ctr &t_ctr) const {
    return m_impl->column().get(t_ctr);
}

unsigned int Var::index() const {
    return m_impl->index();
}

unsigned int Var::model_id() const {
    return m_impl->model_id();
}
