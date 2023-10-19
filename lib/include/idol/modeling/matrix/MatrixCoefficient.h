//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MATRIXCOEFFICIENT_H
#define OPTIMIZE_MATRIXCOEFFICIENT_H

#include "MatrixCoefficientReference.h"

namespace idol {
    class MatrixCoefficient;
}

class idol::MatrixCoefficient : public MatrixCoefficientReference {
public:
    explicit MatrixCoefficient(Constant&& t_coefficient);
    explicit MatrixCoefficient(const Constant& t_coefficient);

    MatrixCoefficient(const MatrixCoefficient&) = delete;
    MatrixCoefficient(MatrixCoefficient&&) noexcept = default;

    MatrixCoefficient& operator=(const MatrixCoefficient&) = delete;
    MatrixCoefficient& operator=(MatrixCoefficient&&) noexcept = default;

    bool is_ref() const override { return false; }

    ~MatrixCoefficient() override;
};

#endif //OPTIMIZE_MATRIXCOEFFICIENT_H
