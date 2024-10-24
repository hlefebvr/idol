//
// Created by henri on 04/10/22.
//
#include "idol/modeling/matrix/Row.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/modeling/solutions/Solution.h"

#include <memory>

const idol::Row idol::Row::EmptyRow;

double idol::Row::value(const idol::Solution::Primal &t_primals) const {

    double result = -rhs();

    for (const auto& [var, constant] : linear()) {
        result += constant * t_primals.get(var);
    }

    for (const auto& [var1, var2, constant] : quadratic()) {
        result += constant * t_primals.get(var1) * t_primals.get(var2);
    }

    return result;
}

bool idol::Row::is_violated(const idol::Solution::Primal &t_primals, idol::CtrType t_type, double t_tolerance) const {

    const double value = this->value(t_primals);

    if (t_type == LessOrEqual) {
        return value > t_tolerance;
    }

    if (t_type == GreaterOrEqual) {
        return value < -t_tolerance;
    }

    return !equals(value, 0, t_tolerance);
}

idol::impl::Row::Row(::idol::Expr<Var> &&t_lhs, ::idol::Expr<Var> &&t_rhs)
    : m_impl(
            std::move(t_lhs.linear()) - t_rhs.linear(),
            std::move(t_lhs.quadratic()) - t_rhs.quadratic(),
            t_rhs.constant() - t_lhs.constant()
        ) {

}

idol::impl::Row::Row(::idol::Expr<Var> &&t_lhs, const ::idol::Expr<Var> &t_rhs)
        : m_impl(
                std::move(t_lhs.linear()) - t_rhs.linear(),
                std::move(t_lhs.quadratic()) - t_rhs.quadratic(),
                t_rhs.constant() - t_lhs.constant()
            ) {

}

idol::impl::Row::Row(const ::idol::Expr<Var> &t_lhs, ::idol::Expr<Var> &&t_rhs)
    : m_impl(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        t_rhs.constant() - t_lhs.constant()
    ) {

}

idol::impl::Row::Row(const ::idol::Expr<Var> &t_lhs, const ::idol::Expr<Var> &t_rhs)
    : m_impl(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        t_rhs.constant() - t_lhs.constant()
    ) {

}

double idol::impl::Row::gcd() const {
    return m_impl.gcd();
}
