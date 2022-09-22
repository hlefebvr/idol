//
// Created by henri on 07/09/22.
//
#include "modeling/constraints/Constraint.h"
#include "modeling/constraints/impl_Constraint.h"
#include "solvers/solutions/Solution.h"

unsigned int Ctr::id() const {
    return m_impl->id();
}

const std::string &Ctr::name() const {
    return m_impl->name();
}

const Row &Ctr::row() const {
    return m_impl->row();
}

const Coefficient &Ctr::rhs() const {
    return m_impl->row().constant();
}

const Coefficient &Ctr::get(const Var &t_var) const {
    return m_impl->row().get(t_var);
}

CtrType Ctr::type() const {
    return m_impl->type();
}

unsigned int Ctr::index() const {
    return m_impl->index();
}

unsigned int Ctr::model_id() const {
    return m_impl->model_id();
}

bool Ctr::is_violated(const Solution::Primal &t_solution) const {
    const double rhs = m_impl->row().constant().constant();
    double lhs = 0.;
    for (const auto& [var, coeff] : m_impl->row()) {
        lhs += coeff.constant() * t_solution.get(var);
    }
    switch (m_impl->type()) {
        case LessOrEqual: return lhs <= rhs;
        case GreaterOrEqual: return lhs >= rhs;
        default:;
    }
    return equals(lhs, rhs, ToleranceForIntegrality);
}

std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr) {
    t_os << t_ctr.name() << " : ";
    t_os << (AbstractExpr<Var>&) t_ctr.row();
    switch (t_ctr.type()) {
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
        case Equal: t_os << " == "; break;
    }
    return t_os << t_ctr.rhs();
}