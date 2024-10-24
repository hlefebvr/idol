//
// Created by henri on 22/02/23.
//

#ifndef IDOL_TO_ROTATED_QUADRATIC_CONE_H
#define IDOL_TO_ROTATED_QUADRATIC_CONE_H

#ifdef IDOL_USE_EIGEN

#include "idol/modeling/expressions/QuadExpr.h"
#include "idol/modeling/expressions/LinExpr.h"
#include "SquareMatrix.h"

namespace idol {

    static std::list<Expr<Var>> to_rotated_quadratic_cone(const QuadExpr<Var> &t_expr) {

        // Create index mapping
        MatrixIndices indices;

        for (const auto &[var1, var2, constant]: t_expr) {
            indices.add(var1);
            indices.add(var2);
        }

        // Create quadratic form matrix s.t. quadratic_part = x^TQx
        SquareMatrix Q(indices);

        for (const auto &[var1, var2, constant]: t_expr) {
            if (var1.id() == var2.id()) {
                Q.set(var1, var2, constant);
            } else {
                Q.set(var1, var2, constant / 2.);
            }
        }

        // Compute Q = LDL^T decomposition
        const auto &[L, D] = Q.eigen_decomposition();

        // Search for negative eigen values
        std::list<std::pair<Var, double>> negative_eigen_values;
        for (const auto &[var, index]: indices.indices()) {
            const double eigen_value = D.get(index, index);
            if (eigen_value < 0) {
                negative_eigen_values.emplace_back(var, eigen_value);
            }
        }

        const unsigned int n_negative_eigen_values = negative_eigen_values.size();

        if (n_negative_eigen_values > 2) {
            throw Exception("Non-convex expression found.");
        }

        // We write x^TQx = ||Fx||^2 + x^TNx

        // Compute F = sqrt( D - D^- )L^T where D^- is D without negative entries
        auto F = D;
        for (const auto &[var, eigen_value]: negative_eigen_values) {
            F.set(var, var, 0);
        }
        F = F.sqrt() * L.transpose();

        // Compute N = LD^-L^T
        SquareMatrix N(indices);
        for (const auto &[var, eigen_value]: negative_eigen_values) {
            N.set(var, var, eigen_value);
        }
        N = L * N * L.transpose();

        // Build result
        std::list<Expr<Var>> result;

        // Create head
        for (const auto &[var1, i]: indices.indices()) {
            LinExpr<Var> expr;
            for (const auto &[var2, j]: indices.indices()) {
                const double n_ij = N.get(i, j);
                if (n_ij == 0) { continue; }
                expr += LinExpr<Var>(std::sqrt(-n_ij), var2);
            }
            if (!expr.empty()) {
                result.emplace_back(std::move(expr));
            }
        }

        if (result.empty()) {
            result.emplace_back();
            result.emplace_back();
        }

        if (result.size() == 1) {
            result.emplace_back(result.front());
        }

        result.front() *= .5;

        if (result.size() > 2) {
            throw Exception("Non-convex expression found.");
        }

        for (const auto &[_, i]: indices.indices()) {

            LinExpr<Var> expr;
            for (const auto &[var, j]: indices.indices()) {
                expr += LinExpr<Var>(F.get(i, j), var);
            }

            if (!expr.empty()) {
                result.emplace_back(std::move(expr));
            }
        }

        return result;
    }

}

#endif

#endif //IDOL_TO_ROTATED_QUADRATIC_CONE_H
