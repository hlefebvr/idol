//
// Created by henri on 22/02/23.
//

#ifndef IDOL_SQUAREMATRIX_H
#define IDOL_SQUAREMATRIX_H

#ifdef IDOL_USE_EIGEN

#include "MatrixIndices.h"
#include <Eigen/Eigen>

class SquareMatrix {
    const MatrixIndices* m_indices;
    Eigen::MatrixXd m_matrix;

    SquareMatrix(const MatrixIndices& t_indices, Eigen::MatrixXd t_matrix) : m_indices(&t_indices), m_matrix(std::move(t_matrix)) {}
public:
    explicit SquareMatrix(const MatrixIndices& t_indices) : m_indices(&t_indices), m_matrix(Eigen::MatrixXd::Zero(t_indices.n_indices(), t_indices.n_indices())) {}

    void set(const Var& t_var1, const Var& t_var2, double t_value) {
        const unsigned int i = m_indices->get(t_var1);
        const unsigned int j = m_indices->get(t_var2);
        m_matrix(i, j) = t_value;
        m_matrix(j, i) = t_value;
    }

    [[nodiscard]] double get(unsigned int t_i, unsigned int t_j) const { return m_matrix(t_i, t_j); }

    [[nodiscard]] double get(const Var& t_i, const Var& t_j) const { return m_matrix(m_indices->get(t_i), m_indices->get(t_j)); }

    [[nodiscard]] unsigned int size() const { return m_indices->n_indices(); }

    [[nodiscard]] SquareMatrix operator*(const SquareMatrix& t_matrix) const {
        auto result = *this;
        result.m_matrix *= t_matrix.m_matrix;
        return result;
    }

    [[nodiscard]] SquareMatrix transpose() const {
        return { *m_indices, m_matrix.transpose() };
    }

    [[nodiscard]] SquareMatrix sqrt() const {
        assert(m_matrix.isDiagonal());
        auto result = *this;
        for (const auto& [var, index] : m_indices->indices()) {
            result.m_matrix(index, index) = std::sqrt(result.m_matrix(index, index));
        }
        return result;
    }

    [[nodiscard]] std::pair<SquareMatrix, SquareMatrix> eigen_decomposition() const {
        Eigen::EigenSolver<Eigen::MatrixXd> solver(m_matrix);
        return std::make_pair(
                SquareMatrix(*m_indices, solver.pseudoEigenvectors()),
                SquareMatrix(*m_indices, solver.pseudoEigenvalueMatrix())
        );
    }

    [[nodiscard]] SquareMatrix cholesky() const {
        Eigen::LLT<Eigen::MatrixXd> solver(m_matrix);
        return SquareMatrix(*m_indices, solver.matrixL());
    }
};

static std::ostream& operator<<(std::ostream& t_os, const SquareMatrix& t_matrix) {
    for (unsigned int i = 0, n = t_matrix.size() ; i < n ; ++i) {
        for (unsigned int j = 0 ; j < n ; ++j) {
            t_os << t_matrix.get(i, j) <<", ";
        }
        t_os << '\n';
    }
    return t_os;
}

#endif

#endif //IDOL_SQUAREMATRIX_H
