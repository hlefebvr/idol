//
// Created by henri on 08/09/22.
//
#include "idol/modeling/matrix/MatrixCoefficientReference.h"
#include "idol/modeling/expressions/Constant.h"

void idol::MatrixCoefficientReference::set_value(double t_coefficient) {
    *m_coefficient = t_coefficient;
}

idol::MatrixCoefficientReference &idol::MatrixCoefficientReference::operator*=(double t_factor) {
    *m_coefficient *= t_factor;
    return *this;
}

idol::MatrixCoefficientReference &idol::MatrixCoefficientReference::operator+=(const AbstractMatrixCoefficient &t_rhs) {
    *m_coefficient += t_rhs.value();
    return *this;
}

