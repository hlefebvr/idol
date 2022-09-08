//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MATRIXCOEFFICIENT_H
#define OPTIMIZE_MATRIXCOEFFICIENT_H

#include "MatrixCoefficientReference.h"

class MatrixCoefficient : public MatrixCoefficientReference {
public:
    explicit MatrixCoefficient(Coefficient&& t_coefficient);
    explicit MatrixCoefficient(const Coefficient& t_coefficient);

    MatrixCoefficient(const MatrixCoefficient&) = delete;
    MatrixCoefficient(MatrixCoefficient&&) noexcept = default;

    MatrixCoefficient& operator=(const MatrixCoefficient&) = delete;
    MatrixCoefficient& operator=(MatrixCoefficient&&) noexcept = default;

    ~MatrixCoefficient() override;
};

#endif //OPTIMIZE_MATRIXCOEFFICIENT_H
