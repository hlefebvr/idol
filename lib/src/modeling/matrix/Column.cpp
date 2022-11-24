//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/matrix/Column.h"
#include "../../../include/modeling/expressions/operations/operators_Ctr.h"

const Column Column::EmptyColumn;

Column::Column() {

}

Column::Column(Constant t_rhs)
        : m_components(std::move(t_rhs)) {

}

Column::Column(const Column &t_src) : m_components(t_src.m_components) {

}

Column &Column::operator=(const Column &t_src) {
    if (this == &t_src) { return *this; }
    m_components = t_src.m_components;
    return *this;
}

void Column::set_components(LinExpr<Ctr> t_components) {
    m_components = std::move(t_components);
}

void Column::set_objective_coefficient(Constant t_objective_coefficient) {
    m_components.constant() = std::move(t_objective_coefficient);
}

void Column::set_objective_coefficient(MatrixCoefficientReference &&t_objective_coefficient) {
    m_components.set_constant(std::move(t_objective_coefficient));
}

Column Column::fix(const Solution::Primal &t_primals) const {
    Column result;

    for (const auto& [var, constant] : m_components.linear()) {
        result.m_components += constant.fix(t_primals) * var;
    }

    result.m_components.constant() = m_components.constant().fix(t_primals);

    return result;
}
