//
// Created by henri on 04/10/22.
//
#include "modeling/expressions/Row2.h"

const Row2 Row2::EmptyRow;

Row2::Row2(LinExpr<Var> t_lhs, Constant t_rhs)
    : m_lhs(std::move(t_lhs)),
      m_rhs(new MatrixCoefficient(std::move(t_rhs))) {

}

Row2::Row2(const Row2 &t_src)
    : m_lhs(t_src.m_lhs),
      m_rhs(t_src.m_rhs ? std::make_unique<MatrixCoefficient>(t_src.m_rhs->value()) : std::unique_ptr<MatrixCoefficient>()) {

}

Row2 &Row2::operator=(const Row2 &t_src) {
    if (this == &t_src) { return *this; }
    m_lhs = t_src.m_lhs;
    m_rhs.reset(t_src.m_rhs ? new MatrixCoefficient(t_src.m_rhs->value()) : nullptr);
    return *this;
}

