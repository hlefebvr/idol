//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/variables/Variable.h"
#include "../../../include/modeling/variables/impl_Variable.h"

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

const Constant& Var::obj() const {
    return m_impl->column().objective_coefficient();
}

const Constant &Var::get(const Ctr &t_ctr) const {
    return m_impl->column().components().get(t_ctr);
}

unsigned int Var::index() const {
    return m_impl->index();
}

unsigned int Var::model_id() const {
    return m_impl->model_id();
}
