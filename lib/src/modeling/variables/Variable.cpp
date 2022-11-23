//
// Created by henri on 07/09/22.
//
#include <utility>

#include "../../../include/modeling/variables/Var.h"
#include "../../../include/modeling/variables/impl_Variable.h"
#include "../../../include/modeling/environment/Env.h"

Var::Var(const std::shared_ptr<impl::Var>& t_impl) : m_impl(t_impl) {

}

const Column &Var::column() const {
    return m_impl.lock()->column();
}

double Var::lb() const {
    return m_impl.lock()->lb();
}

double Var::ub() const {
    return m_impl.lock()->ub();
}

VarType Var::type() const {
    return m_impl.lock()->type();
}

const Constant& Var::obj() const {
    return m_impl.lock()->column().objective_coefficient();
}

const Constant &Var::get(const Ctr &t_ctr) const {
    return m_impl.lock()->column().components().linear().get(t_ctr);
}

impl::Object &Var::impl() {
    return *m_impl.lock();
}

const impl::Object &Var::impl() const {
    return *m_impl.lock();
}

ObjectStatus Var::status() const {
    return m_impl.expired() ? Dead : Alive;
}
