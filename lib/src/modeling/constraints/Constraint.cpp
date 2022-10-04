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

const Constant &Ctr::rhs() const {
    return m_impl->row().rhs();
}

const Constant &Ctr::get(const Var &t_var) const {
    return m_impl->row().lhs().get(t_var);
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
    const double rhs = m_impl->row().rhs().constant();
    double lhs = 0.;
    for (const auto& [var, coeff] : m_impl->row().lhs()) {
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
    return t_os << "TODO";
}