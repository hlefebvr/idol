//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MATRIXCOEFFICIENTREFERENCE_H
#define OPTIMIZE_MATRIXCOEFFICIENTREFERENCE_H

#include "AbstractMatrixCoefficient.h"

class Coefficient;

class MatrixCoefficientReference : public AbstractMatrixCoefficient {
protected:
    Coefficient* m_coefficient = nullptr;
    explicit MatrixCoefficientReference(Coefficient* t_coefficient) : m_coefficient(t_coefficient) {}
public:
    MatrixCoefficientReference() = default;
    explicit MatrixCoefficientReference(AbstractMatrixCoefficient& t_src) : m_coefficient(&t_src.value()) {}

    MatrixCoefficientReference(const MatrixCoefficientReference&) = delete;
    MatrixCoefficientReference(MatrixCoefficientReference&&) noexcept = default;

    MatrixCoefficientReference& operator=(const MatrixCoefficientReference&) = delete;
    MatrixCoefficientReference& operator=(MatrixCoefficientReference&&) noexcept = default;

    [[nodiscard]] bool empty() const { return !m_coefficient; }

    void set_value(Coefficient &&t_coefficient) override;

    Coefficient &value() override { return *m_coefficient; }

    [[nodiscard]] const Coefficient &value() const override { return *m_coefficient; }

    MatrixCoefficientReference &operator*=(double t_factor) override;

    MatrixCoefficientReference &operator+=(const AbstractMatrixCoefficient &t_rhs) override;
};

#endif //OPTIMIZE_MATRIXCOEFFICIENTREFERENCE_H
