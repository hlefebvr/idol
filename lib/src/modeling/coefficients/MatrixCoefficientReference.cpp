//
// Created by henri on 08/09/22.
//
#include "modeling/coefficients/MatrixCoefficientReference.h"
#include "modeling/coefficients/Coefficient.h"

void MatrixCoefficientReference::set_value(Coefficient &&t_coefficient) {
    *m_coefficient = std::move(t_coefficient);
}

