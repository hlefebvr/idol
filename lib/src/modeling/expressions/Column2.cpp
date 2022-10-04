//
// Created by henri on 04/10/22.
//
#include "modeling/expressions/Column2.h"

const Column2 Column2::EmptyColumn;

Column2::Column2(Constant t_rhs)
        : m_objective_coefficient(new MatrixCoefficient(std::move(t_rhs))) {

}

Column2::Column2(const Column2 &t_src)
        : m_components(t_src.m_components),
          m_objective_coefficient(t_src.m_objective_coefficient ? std::make_unique<MatrixCoefficient>(t_src.m_objective_coefficient->value()) : std::unique_ptr<MatrixCoefficient>()) {

}

Column2 &Column2::operator=(const Column2 &t_src) {
    if (this == &t_src) { return *this; }
    m_components = t_src.m_components;
    m_objective_coefficient.reset(t_src.m_objective_coefficient ? new MatrixCoefficient(t_src.m_objective_coefficient->value()) : nullptr);
    return *this;
}

