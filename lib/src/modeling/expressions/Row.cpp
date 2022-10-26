//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/expressions/Row.h"
#include "../../../include/modeling/expressions/operators.h"

#include <memory>

const Row Row::EmptyRow;

Row::Row() : m_rhs(std::make_unique<MatrixCoefficient>(0.)) {

}

Row::Row(LinExpr<Var> t_lhs, Constant t_rhs)
    : m_lhs(std::move(t_lhs)),
      m_rhs(std::make_unique<MatrixCoefficient>(std::move(t_rhs))) {

}

Row::Row(const Row &t_src)
    : m_lhs(t_src.m_lhs),
      m_rhs(t_src.m_rhs ? std::make_unique<MatrixCoefficient>(t_src.m_rhs->value()) : std::make_unique<MatrixCoefficient>(0.)) {

}

Row &Row::operator=(const Row &t_src) {
    if (this == &t_src) { return *this; }
    m_lhs = t_src.m_lhs;
    m_rhs->value() = t_src.m_rhs->value();
    return *this;
}

void Row::set_lhs(LinExpr<Var> t_lhs) {
    m_lhs = std::move(t_lhs);
}

void Row::set_rhs(Constant t_rhs) {
    m_rhs->value() = std::move(t_rhs);
}

Row Row::operator*=(double t_factor) {
    m_rhs->value() *= t_factor;
    m_lhs *= t_factor;
    return *this;
}

Row Row::operator+=(const Row &t_factor) {
    m_rhs->value() += t_factor.rhs();
    m_lhs += t_factor.m_lhs;
    return *this;
}

Row Row::transpose() const {
    Row result;

    for (const auto& [var, constant] : m_lhs) {

        result.rhs() -= constant.numerical() * !var;

        for (const auto& [param, coeff] : constant) {
            result.lhs() += -coeff * !var * param.as<Var>();
        }

    }

    for (const auto& [param, coeff] : m_rhs->value()) {
        result.lhs() -= coeff * param.as<Var>();
    }

    return result;
}

Row Row::fix(const Solution::Primal &t_primals) const {
    Row result;

    for (const auto& [var, constant] : m_lhs) {
        result.m_lhs += constant.fix(t_primals) * var;
    }

    result.m_rhs->set_value(m_rhs->value().fix(t_primals));

    return result;
}

Row::Row(Expr<Var> &&t_lhs, Expr<Var> &&t_rhs)
    : m_lhs(std::move(t_lhs.linear()) - t_lhs.linear()),
      m_rhs(std::make_unique<MatrixCoefficient>(std::move(t_rhs.constant()) - t_lhs.constant())) {

}

Row::Row(Expr<Var> &&t_lhs, const Expr<Var> &t_rhs)
    : m_lhs(std::move(t_lhs.linear()) - t_lhs.linear()),
      m_rhs(std::make_unique<MatrixCoefficient>(t_rhs.constant() - t_lhs.constant())){

}

Row::Row(const Expr<Var> &t_lhs, Expr<Var> &&t_rhs)
    : m_lhs(t_lhs.linear() - t_lhs.linear()),
      m_rhs(std::make_unique<MatrixCoefficient>(std::move(t_rhs.constant()) - t_lhs.constant())){

}

Row::Row(const Expr<Var> &t_lhs, const Expr<Var> &t_rhs)
    : m_lhs(t_lhs.linear() - t_lhs.linear()),
      m_rhs(std::make_unique<MatrixCoefficient>(t_rhs.constant() - t_lhs.constant())) {

}
