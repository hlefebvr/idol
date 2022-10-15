//
// Created by henri on 07/09/22.
//
#include <utility>

#include "../../../include/modeling/variables/Variable.h"
#include "../../../include/modeling/variables/impl_Variable.h"

Var::Var(impl::Var* t_impl) : m_impl(t_impl) {

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

impl::Object &Var::impl() {
    return *m_impl;
}

const impl::Object &Var::impl() const {
    return *m_impl;
}

Var::~Var() {

    if (m_impl == nullptr) {
        return;
    }

    --m_impl->count();

    if (m_impl->count() == 0) {
        delete m_impl;
    }

}

Var::Var(const Var &t_var) : m_impl(t_var.m_impl) {
    ++m_impl->count();
}

Var::Var(Var &&t_var) noexcept : m_impl(t_var.m_impl) {
    t_var.m_impl = nullptr;
}

Var &Var::operator=(const Var &t_var) {
    if (this == &t_var) { return *this; }
    m_impl = t_var.m_impl;
    ++m_impl->count();
    return *this;
}

Var &Var::operator=(Var &&t_var) noexcept {
    if (this == &t_var) { return *this; }
    m_impl = t_var.m_impl;
    t_var.m_impl = nullptr;
    return *this;
}
