//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_H
#define IDOL_OPERATORS_H

#include "operators_Constant.h"
#include "operators_Var.h"
#include "operators_Ctr.h"
#include "operators_utils.h"

namespace idol {
    template<class T>
    double evaluate(const LinExpr<T>& t_expr, const Point<T>& t_values) {
        double result = 0.;

        for (const auto& [var, constant] : t_expr) {
            result += constant * t_values.get(var);
        }

        return result;
    }

    template<class T>
    double evaluate(const Expr<T>& t_expr, const Point<T>& t_values) {
        return t_expr.constant() + evaluate(t_expr.linear(), t_values);
    }

    template<class KeyT, class ValueT, class IndexExtractorT, class PointT>
    LinExpr<KeyT> evaluate(const SparseVector<KeyT, ValueT, IndexExtractorT>& t_pattern, const PointT& t_values) {
        LinExpr<KeyT> result;
        result.reserve_at_least(t_pattern.size());
        for (const auto& [key, value] : t_pattern) {
            result.push_back(key, evaluate(value, t_values));
        }
        return result;
    }

}

#endif //IDOL_OPERATORS_H
