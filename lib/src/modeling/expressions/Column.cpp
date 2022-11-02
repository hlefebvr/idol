//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/expressions/Column.h"
#include "../../../include/modeling/expressions/operators_Ctr.h"

const Column Column::EmptyColumn;

Column::Column() : m_objective_coefficient(std::make_unique<MatrixCoefficient>(0.)) {

}

Column::Column(Constant t_rhs)
        : m_objective_coefficient(new MatrixCoefficient(std::move(t_rhs))) {

}

Column::Column(const Column &t_src)
        : m_components(t_src.m_components),
          m_objective_coefficient(t_src.m_objective_coefficient ? std::make_unique<MatrixCoefficient>(t_src.m_objective_coefficient->value()) : std::make_unique<MatrixCoefficient>(0.)) {

}

Column &Column::operator=(const Column &t_src) {
    if (this == &t_src) { return *this; }
    m_components = t_src.m_components;
    m_objective_coefficient->value() = t_src.m_objective_coefficient->value();
    return *this;
}

void Column::set_components(LinExpr<Ctr> t_components) {
    m_components = std::move(t_components);
}

void Column::set_objective_coefficient(Constant t_objective_coefficient) {
    m_objective_coefficient->value() = std::move(t_objective_coefficient);
}

void Column::set_objective_coefficient(MatrixCoefficientReference &&t_objective_coefficient) {
    m_objective_coefficient = std::make_unique<MatrixCoefficientReference>(std::move(t_objective_coefficient));
}

Column Column::fix(const Solution::Primal &t_primals) const {
    Column result;

    for (const auto& [var, constant] : m_components) {
        result.m_components += constant.fix(t_primals) * var;
    }

    result.m_objective_coefficient->set_value(m_objective_coefficient->value().fix(t_primals));

    return result;
}
