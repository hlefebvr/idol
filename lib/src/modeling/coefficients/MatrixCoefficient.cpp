//
// Created by henri on 07/09/22.
//
#include "modeling/coefficients/MatrixCoefficient.h"
#include "modeling/coefficients/Coefficient.h"

MatrixCoefficient::~MatrixCoefficient() {
    delete m_coefficient;
}

MatrixCoefficient::MatrixCoefficient(Coefficient &&t_coefficient)
    : MatrixCoefficientReference(new Coefficient(std::move(t_coefficient))) {

}

MatrixCoefficient::MatrixCoefficient(const Coefficient &t_coefficient)
        : MatrixCoefficientReference(new Coefficient(t_coefficient)){

}

