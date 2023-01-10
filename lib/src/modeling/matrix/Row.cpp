//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/matrix/Row.h"
#include "../../../include/modeling/expressions/operations/operators.h"

#include <memory>

const Row Row::EmptyRow;

Row Row::fix(const Solution::Primal &t_primals) const {
    Row result;

    for (const auto& [var, constant] : linear()) {
        result.linear() += constant.fix(t_primals) * var;
    }

    result.rhs() = rhs().fix(t_primals);

    return result;
}

Row Row::fix(const Solution::Dual &t_duals) const {
    Row result;

    for (const auto& [var, constant] : linear()) {
        result.linear() += constant.fix(t_duals) * var;
    }

    result.rhs() = rhs().fix(t_duals);

    return result;
}

impl::Row::Row(::Expr<Var> &&t_lhs, ::Expr<Var> &&t_rhs)
    : m_impl(
            std::move(t_lhs.linear()) - t_rhs.linear(),
            std::move(t_lhs.quadratic()) - t_rhs.quadratic(),
            std::move(t_rhs.constant()) - t_lhs.constant()
        ) {

}

impl::Row::Row(::Expr<Var> &&t_lhs, const ::Expr<Var> &t_rhs)
        : m_impl(
                std::move(t_lhs.linear()) - t_rhs.linear(),
                std::move(t_lhs.quadratic()) - t_rhs.quadratic(),
                t_rhs.constant() - t_lhs.constant()
            ) {

}

impl::Row::Row(const ::Expr<Var> &t_lhs, ::Expr<Var> &&t_rhs)
    : m_impl(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        std::move(t_rhs.constant()) - t_lhs.constant()
    ) {

}

impl::Row::Row(const ::Expr<Var> &t_lhs, const ::Expr<Var> &t_rhs)
    : m_impl(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        t_rhs.constant() - t_lhs.constant()
    ) {

}
