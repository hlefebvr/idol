//
// Created by henri on 07/09/22.
//
#include <utility>

#include "../../../include/modeling/constraints/Constraint.h"
#include "../../../include/modeling/constraints/impl_Constraint.h"
#include "../../../include/modeling/solutions/Solution.h"
#include "../../../include/modeling/environment/Env.h"

Ctr::Ctr(const std::shared_ptr<impl::Ctr>& t_impl) : m_impl(t_impl) {

}

const Row &Ctr::row() const {
    return m_impl.lock()->row();
}

const Constant &Ctr::rhs() const {
    return m_impl.lock()->row().rhs();
}

const Constant &Ctr::get(const Var &t_var) const {
    return m_impl.lock()->row().lhs().get(t_var);
}

CtrType Ctr::type() const {
    return m_impl.lock()->type();
}

bool Ctr::is_violated(const Solution::Primal &t_solution) const {
    const double rhs = m_impl.lock()->row().rhs().numerical();
    double lhs = 0.;
    for (const auto& [var, coeff] : m_impl.lock()->row().lhs()) {
        lhs += coeff.numerical() * t_solution.get(var);
    }
    switch (m_impl.lock()->type()) {
        case LessOrEqual: return lhs <= rhs;
        case GreaterOrEqual: return lhs >= rhs;
        default:;
    }
    return equals(lhs, rhs, ToleranceForIntegrality);
}

impl::Object &Ctr::impl() {
    return *m_impl.lock();
}

const impl::Object &Ctr::impl() const {
    return *m_impl.lock();
}

ObjectStatus Ctr::status() const {
    return m_impl.expired() ? Removed : InModel;
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