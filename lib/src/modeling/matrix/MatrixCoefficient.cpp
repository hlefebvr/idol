//
// Created by henri on 07/09/22.
//
#include "idol/modeling/matrix/MatrixCoefficient.h"
#include "idol/modeling/expressions/Constant.h"

idol::MatrixCoefficient::~MatrixCoefficient() {
    delete m_coefficient;
}

idol::MatrixCoefficient::MatrixCoefficient(double t_coefficient)
        : MatrixCoefficientReference(new double(t_coefficient)){

}

