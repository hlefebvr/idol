//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_H
#define IDOL_OPERATORS_H

#include "operators_Constant.h"
#include "operators_Var.h"
#include "operators_Ctr.h"
#include "operators_utils.h"
#include "idol/general/utils/Point.h"
#include <cassert>
#include <idol/mixed-integer/modeling/expressions/QuadExpr.h>

namespace idol {
    template<class T>
    double evaluate(const LinExpr<T, double>& t_expr, const Point<T>& t_values) {
        double result = 0.;

        for (const auto& [var, constant] : t_expr) {
            result += constant * t_values.get(var);
        }

        return result;
    }

    template<class T>
    double evaluate(const AffExpr<T, double>& t_expr, const Point<T>& t_values) {
        return t_expr.constant() + evaluate(t_expr.linear(), t_values);
    }

    template<class T>
    double evaluate(const QuadExpr<T, double>& t_expr, const Point<T>& t_values) {
        double result = evaluate(t_expr.affine(), t_values);

        for (const auto& [pair, constant] : t_expr) {
            result += constant * t_values.get(pair.first) * t_values.get(pair.second);
        }

        return result;
    }

    template<class KeyT, class ValueT, class PointT>
    LinExpr<KeyT> evaluate(const SparseVector<KeyT, ValueT>& t_pattern, const PointT& t_values) {
        LinExpr<KeyT> result;
        result.reserve(t_pattern.size());
        for (const auto& [key, value] : t_pattern) {
            result.set(key, evaluate(value, t_values));
        }
        return result;
    }

}

#endif //IDOL_OPERATORS_H
