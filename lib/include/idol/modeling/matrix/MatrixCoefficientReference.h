//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MATRIXCOEFFICIENTREFERENCE_H
#define OPTIMIZE_MATRIXCOEFFICIENTREFERENCE_H

#include "AbstractMatrixCoefficient.h"



namespace idol {
    class Constant;
    class MatrixCoefficientReference;
}

class idol::MatrixCoefficientReference : public AbstractMatrixCoefficient {
protected:
    double* m_coefficient = nullptr;
    explicit MatrixCoefficientReference(double* t_src) : m_coefficient(t_src) {}
public:
    explicit MatrixCoefficientReference(AbstractMatrixCoefficient& t_src) : m_coefficient(&t_src.value()) {}

    MatrixCoefficientReference(const MatrixCoefficientReference&) = delete;
    MatrixCoefficientReference(MatrixCoefficientReference&&) noexcept = default;

    MatrixCoefficientReference& operator=(const MatrixCoefficientReference&) = delete;
    MatrixCoefficientReference& operator=(MatrixCoefficientReference&&) noexcept = default;

    bool is_ref() const override { return true; }

    [[nodiscard]] bool empty() const { return !m_coefficient; }

    void set_value(double t_coefficient) override;

    double &value() override { return *m_coefficient; }

    [[nodiscard]] double value() const override { return *m_coefficient; }

    MatrixCoefficientReference &operator*=(double t_factor) override;

    MatrixCoefficientReference &operator+=(const AbstractMatrixCoefficient &t_rhs) override;
};

#endif //OPTIMIZE_MATRIXCOEFFICIENTREFERENCE_H
