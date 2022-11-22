//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/expressions/Row.h"
#include "../../../include/modeling/expressions/operators.h"

#include <memory>

const Row Row::EmptyRow;

void Row::set_lhs(LinExpr<Var> t_lhs) {
    m_lhs = std::move(t_lhs);
}

void Row::set_rhs(Constant t_rhs) {
    m_lhs.constant() = std::move(t_rhs);
}

Row Row::operator*=(double t_factor) {
    m_lhs *= t_factor;
    return *this;
}

Row Row::operator+=(const Row &t_factor) {
    m_lhs += t_factor.m_lhs;
    return *this;
}

Row Row::transpose() const {
    Row result;

    for (const auto& [var, constant] : m_lhs.linear()) {

        result.rhs() -= constant.numerical() * !var;

        for (const auto& [param, coeff] : constant) {
            result.lhs() += -coeff * !var * param.as<Var>();
        }

    }

    for (const auto& [param, coeff] : m_lhs.constant()) {
        result.lhs() -= coeff * param.as<Var>();
    }

    return result;
}

Row Row::fix(const Solution::Primal &t_primals) const {
    Row result;

    for (const auto& [var, constant] : m_lhs.linear()) {
        result.m_lhs += constant.fix(t_primals) * var;
    }

    result.m_lhs.constant() = m_lhs.constant().fix(t_primals);

    return result;
}

Row::Row(Expr<Var> &&t_lhs, Expr<Var> &&t_rhs)
    : m_lhs(
            std::move(t_lhs.linear()) - t_rhs.linear(),
            std::move(t_lhs.quadratic()) - t_rhs.quadratic(),
            std::move(t_rhs.constant()) - t_lhs.constant()
        ) {

}

Row::Row(Expr<Var> &&t_lhs, const Expr<Var> &t_rhs)
        : m_lhs(
                std::move(t_lhs.linear()) - t_rhs.linear(),
                std::move(t_lhs.quadratic()) - t_rhs.quadratic(),
                t_rhs.constant() - t_lhs.constant()
            ) {

}

Row::Row(const Expr<Var> &t_lhs, Expr<Var> &&t_rhs)
    : m_lhs(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        std::move(t_rhs.constant()) - t_lhs.constant()
    ) {

}

Row::Row(const Expr<Var> &t_lhs, const Expr<Var> &t_rhs)
    : m_lhs(
        t_lhs.linear() - t_rhs.linear(),
        t_lhs.quadratic() - t_rhs.quadratic(),
        t_rhs.constant() - t_lhs.constant()
    ) {

}
