//
// Created by henri on 04/10/22.
//
#include "idol/modeling/matrix/Row.h"
#include "idol/modeling/expressions/operations/operators.h"
#include "idol/modeling/solutions/Solution.h"

#include <memory>

const idol::Row idol::Row::EmptyRow;

idol::Row idol::Row::fix(const Solution::Primal &t_primals) const {
    Row result;

    for (const auto& [var, constant] : linear()) {
        result.linear() += constant.fix(t_primals) * var;
    }

    result.rhs() = rhs().fix(t_primals);

    return result;
}

idol::Row idol::Row::fix(const Solution::Dual &t_duals) const {
    Row result;

    for (const auto& [var, constant] : linear()) {
        result.linear() += constant.fix(t_duals) * var;
    }

    result.rhs() = rhs().fix(t_duals);

    return result;
}

double idol::Row::value(const idol::Solution::Primal &t_primals) const {

    double result = -rhs().numerical();

    for (const auto& [var, constant] : linear()) {
        result += constant.as_numerical() * t_primals.get(var);
    }

    for (const auto& [var1, var2, constant] : quadratic()) {
        result += constant.as_numerical() * t_primals.get(var1) * t_primals.get(var2);
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
            std::move(t_rhs.constant()) - t_lhs.constant()
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
        std::move(t_rhs.constant()) - t_lhs.constant()
    ) {

}

idol::impl::Row::Row(const ::idol::Expr<Var> &t_lhs, const ::idol::Expr<Var> &t_rhs)
    : m_impl(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        t_rhs.constant() - t_lhs.constant()
    ) {

}

double idol::impl::Row::scale_to_integers(unsigned int t_n_significant_digits) {
    return m_impl.scale_to_integers(t_n_significant_digits);
}

void
idol::impl::Row::multiply_with_precision_by_power_of_10(unsigned int t_exponent, unsigned int t_n_significant_digits) {
    m_impl.multiply_with_precision_by_power_of_10(t_exponent, t_n_significant_digits);
}

double idol::impl::Row::gcd() const {
    return m_impl.gcd();
}
