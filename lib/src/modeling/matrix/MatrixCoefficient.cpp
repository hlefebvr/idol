//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/matrix/MatrixCoefficient.h"
#include "../../../include/modeling/expressions/Constant.h"

idol::MatrixCoefficient::~MatrixCoefficient() {
    delete m_coefficient;
}

idol::MatrixCoefficient::MatrixCoefficient(Constant &&t_coefficient)
    : MatrixCoefficientReference(new Constant(std::move(t_coefficient))) {

}

idol::MatrixCoefficient::MatrixCoefficient(const Constant &t_coefficient)
        : MatrixCoefficientReference(new Constant(t_coefficient)){

}

