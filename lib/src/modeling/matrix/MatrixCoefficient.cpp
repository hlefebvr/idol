//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/matrix/MatrixCoefficient.h"
#include "../../../include/modeling/matrix/Constant.h"

MatrixCoefficient::~MatrixCoefficient() {
    delete m_coefficient;
}

MatrixCoefficient::MatrixCoefficient(Constant &&t_coefficient)
    : MatrixCoefficientReference(new Constant(std::move(t_coefficient))) {

}

MatrixCoefficient::MatrixCoefficient(const Constant &t_coefficient)
        : MatrixCoefficientReference(new Constant(t_coefficient)){

}

