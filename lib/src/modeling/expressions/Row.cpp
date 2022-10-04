//
// Created by henri on 04/10/22.
//
#include "modeling/expressions/Row.h"

#include <memory>

const Row Row::EmptyRow;

Row::Row() : m_rhs(std::make_unique<MatrixCoefficient>(0.)) {

}

Row::Row(Expr<Var> t_lhs, Constant t_rhs)
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

void Row::set_lhs(Expr<Var> t_lhs) {
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
