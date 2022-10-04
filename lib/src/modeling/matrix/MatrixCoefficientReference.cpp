//
// Created by henri on 08/09/22.
//
#include "modeling/matrix/MatrixCoefficientReference.h"
#include "modeling/matrix/Constant.h"

void MatrixCoefficientReference::set_value(Constant &&t_coefficient) {
    *m_coefficient = std::move(t_coefficient);
}

MatrixCoefficientReference &MatrixCoefficientReference::operator*=(double t_factor) {
    *m_coefficient *= t_factor;
    return *this;
}

MatrixCoefficientReference &MatrixCoefficientReference::operator+=(const AbstractMatrixCoefficient &t_rhs) {
    *m_coefficient += t_rhs.value();
    return *this;
}

