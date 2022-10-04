//
// Created by henri on 04/10/22.
//
#include "modeling/expressions/Column.h"

const Column Column::EmptyColumn;

Column::Column(Constant t_rhs)
        : m_objective_coefficient(new MatrixCoefficient(std::move(t_rhs))) {

}

Column::Column(const Column &t_src)
        : m_components(t_src.m_components),
          m_objective_coefficient(t_src.m_objective_coefficient ? std::make_unique<MatrixCoefficient>(t_src.m_objective_coefficient->value()) : std::unique_ptr<MatrixCoefficient>()) {

}

Column &Column::operator=(const Column &t_src) {
    if (this == &t_src) { return *this; }
    m_components = t_src.m_components;
    m_objective_coefficient.reset(t_src.m_objective_coefficient ? new MatrixCoefficient(t_src.m_objective_coefficient->value()) : nullptr);
    return *this;
}

void Column::set_components(Expr<Ctr> t_components) {
    m_components = std::move(t_components);
}

void Column::set_objective_coefficient(Constant t_objective_coefficient) {
    m_objective_coefficient = std::make_unique<MatrixCoefficient>(std::move(t_objective_coefficient));
}

