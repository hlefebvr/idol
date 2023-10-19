//
// Created by henri on 04/10/22.
//
#include "idol/modeling/matrix/Row.h"
#include "idol/modeling/expressions/operations/operators.h"

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
