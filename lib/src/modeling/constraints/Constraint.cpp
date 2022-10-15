//
// Created by henri on 07/09/22.
//
#include <utility>

#include "../../../include/modeling/constraints/Constraint.h"
#include "../../../include/modeling/constraints/impl_Constraint.h"
#include "../../../include/modeling/solutions/Solution.h"

Ctr::Ctr(impl::Ctr* t_impl) : m_impl(t_impl) {

}

const Row &Ctr::row() const {
    return m_impl->row();
}

const Constant &Ctr::rhs() const {
    return m_impl->row().rhs();
}

const Constant &Ctr::get(const Var &t_var) const {
    return m_impl->row().lhs().get(t_var);
}

CtrType Ctr::type() const {
    return m_impl->type();
}

bool Ctr::is_violated(const Solution::Primal &t_solution) const {
    const double rhs = m_impl->row().rhs().numerical();
    double lhs = 0.;
    for (const auto& [var, coeff] : m_impl->row().lhs()) {
        lhs += coeff.numerical() * t_solution.get(var);
    }
    switch (m_impl->type()) {
        case LessOrEqual: return lhs <= rhs;
        case GreaterOrEqual: return lhs >= rhs;
        default:;
    }
    return equals(lhs, rhs, ToleranceForIntegrality);
}

impl::Object &Ctr::impl() {
    return *m_impl;
}

const impl::Object &Ctr::impl() const {
    return *m_impl;
}

Ctr::~Ctr() {

    if (m_impl == nullptr) {
        return;
    }

    --m_impl->count();

    if (m_impl->count() == 0) {
        delete m_impl;
    }

}

Ctr::Ctr(const Ctr &t_var) : m_impl(t_var.m_impl) {
    ++m_impl->count();
}

Ctr::Ctr(Ctr &&t_var) noexcept : m_impl(t_var.m_impl) {
    t_var.m_impl = nullptr;
}

Ctr &Ctr::operator=(const Ctr &t_var) {
    if (this == &t_var) { return *this; }
    m_impl = t_var.m_impl;
    ++m_impl->count();
    return *this;
}

Ctr &Ctr::operator=(Ctr &&t_var) noexcept {
    if (this == &t_var) { return *this; }
    m_impl = t_var.m_impl;
    t_var.m_impl = nullptr;
    return *this;
}

std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr) {
    t_os << t_ctr.name() << ": " << t_ctr.row().lhs();
    switch (t_ctr.type()) {
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
        case Equal: t_os << " == "; break;
    }
    return t_os << t_ctr.row().rhs();
}