//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_H
#define IDOL_OPERATORS_H

#include "operators_utils.h"
#include "idol/general/utils/Point.h"
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

    // Products

    // LinExpr
    template<class KeyT>
    LinExpr<KeyT> operator*(double t_num, const KeyT& t_obj) {
        return { t_num, t_obj };
    }

    template<class KeyT>
    LinExpr<KeyT> operator*(const KeyT& t_obj, double t_num) {
        return { t_num, t_obj };
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator*(double t_num, LinExpr<KeyT, ValueT>&& t_lin_expr) {
        LinExpr<KeyT, ValueT> result(std::move(t_lin_expr));
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator*(LinExpr<KeyT, ValueT>&& t_lin_expr, double t_num) {
        LinExpr<KeyT, ValueT> result(std::move(t_lin_expr));
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator*(double t_num, const LinExpr<KeyT, ValueT>& t_lin_expr) {
        LinExpr<KeyT, ValueT> result(t_lin_expr);
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator*(const LinExpr<KeyT, ValueT>& t_lin_expr, double t_num) {
        LinExpr<KeyT, ValueT> result(t_lin_expr);
        result *= t_num;
        return result;
    }

    // AffExpr

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator*(double t_num, AffExpr<KeyT, ValueT>&& t_expr) {
        AffExpr<KeyT, ValueT> result(std::move(t_expr));
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator*(double t_num, const AffExpr<KeyT, ValueT>& t_expr) {
        AffExpr<KeyT, ValueT> result(t_expr);
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator*(AffExpr<KeyT, ValueT>&& t_expr, double t_num) {
        AffExpr<KeyT, ValueT> result(std::move(t_expr));
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator*(const AffExpr<KeyT, ValueT>& t_expr, double t_num) {
        AffExpr<KeyT, ValueT> result(t_expr);
        result *= t_num;
        return result;
    }

    // QuadExpr

    template<class T>
    QuadExpr<T> operator*(const T& t_obj1, const T& t_obj2) {
        return { t_obj1, t_obj2 };
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator*(double t_num, QuadExpr<KeyT, ValueT>&& t_expr) {
        QuadExpr<KeyT, ValueT> result(std::move(t_expr));
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator*(double t_num, const QuadExpr<KeyT, ValueT>& t_expr) {
        QuadExpr<KeyT, ValueT> result(t_expr);
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator*(QuadExpr<KeyT, ValueT>&& t_expr, double t_num) {
        QuadExpr<KeyT, ValueT> result(std::move(t_expr));
        result *= t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator*(const QuadExpr<KeyT, ValueT>& t_expr, double t_num) {
        QuadExpr<KeyT, ValueT> result(t_expr);
        result *= t_num;
        return result;
    }

    // Addition

    // LinExpr

    template<class KeyT>
    LinExpr<KeyT> operator+(const KeyT& t_obj) {
        return { t_obj };
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(LinExpr<KeyT, ValueT>&& t_lin_expr) {
        return std::move(t_lin_expr);
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(const LinExpr<KeyT, ValueT>& t_lin_expr) {
        return t_lin_expr;
    }

    template<class KeyT>
    LinExpr<KeyT> operator+(const KeyT& t_a, const KeyT& t_b) {
        LinExpr<KeyT> result(t_a);
        result += LinExpr<KeyT>(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(LinExpr<KeyT, ValueT>&& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        LinExpr<KeyT, ValueT> result(std::move(t_a));
        result += t_b;
        return result;
   }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(const LinExpr<KeyT, ValueT>& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(LinExpr<KeyT, ValueT>&& t_a, const KeyT& t_b) {
        LinExpr<KeyT, ValueT> result(std::move(t_a));
        result += LinExpr<KeyT, ValueT>(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(const KeyT& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result += std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(const KeyT& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator+(const LinExpr<KeyT, ValueT>& t_a, const KeyT& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result += LinExpr<KeyT, ValueT>(t_b);
        return result;
    }

    // AffExpr

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>&& t_expr) {
        return std::move(t_expr);
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_expr) {
        return t_expr;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(double t_term, LinExpr<KeyT, ValueT>&& t_lin_expr) {
        AffExpr<KeyT, ValueT> result(std::move(t_lin_expr));
        result.constant() += t_term;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(LinExpr<KeyT, ValueT>&& t_lin_expr, double t_term) {
        AffExpr<KeyT, ValueT> result(std::move(t_lin_expr));
        result.constant() += t_term;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const LinExpr<KeyT, ValueT>& t_lin_expr, double t_term) {
        AffExpr<KeyT, ValueT> result(t_lin_expr);
        result.constant() += t_term;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(double t_term, const LinExpr<KeyT, ValueT>& t_lin_expr) {
        AffExpr<KeyT, ValueT> result(t_lin_expr);
        result.constant() += t_term;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_b));
        result.linear() += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(double t_num, AffExpr<KeyT, ValueT>&& t_expr) {
        AffExpr<KeyT, ValueT> result(std::move(t_expr));
        result.constant() += t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(AffExpr<KeyT, ValueT>&& t_expr, double t_num) {
        AffExpr<KeyT, ValueT> result(std::move(t_expr));
        result.constant() += t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_expr, double t_num) {
        AffExpr<KeyT, ValueT> result(t_expr);
        result.constant() += t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(double t_num, const AffExpr<KeyT, ValueT>& t_expr) {
        AffExpr<KeyT, ValueT> result(t_expr);
        result.constant() += t_num;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(AffExpr<KeyT, ValueT>&& t_a, const KeyT& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result.linear() += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const KeyT& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_b));
        result.linear() += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const KeyT& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_b);
        result.linear() += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, const KeyT& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result.linear() += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result += std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(LinExpr<KeyT, ValueT>&& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const LinExpr<KeyT, ValueT>& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_b));
        result += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(AffExpr<KeyT, ValueT>&& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(AffExpr<KeyT, ValueT>&& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result += std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(LinExpr<KeyT, ValueT>&& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result += std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const LinExpr<KeyT, ValueT>& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_b);
        result += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(AffExpr<KeyT, ValueT>&& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result += std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result += t_b;
        return result;
    }

    // QuadExpr

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(QuadExpr<KeyT, ValueT>&& t_expr) {
        return std::move(t_expr);
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const QuadExpr<KeyT, ValueT>& t_expr) {
        return t_expr;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(AffExpr<KeyT, ValueT>&& t_a, const QuadExpr<KeyT, ValueT>& t_b) {
        QuadExpr<KeyT, ValueT> result(std::move(t_a));
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const QuadExpr<KeyT, ValueT>& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        QuadExpr<KeyT, ValueT> result(std::move(t_b));
        result += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, QuadExpr<KeyT, ValueT>&& t_b) {
        QuadExpr<KeyT, ValueT> result(std::move(t_b));
        result += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(QuadExpr<KeyT, ValueT>&& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        QuadExpr<KeyT, ValueT> result(std::move(t_a));
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const QuadExpr<KeyT, ValueT>& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        QuadExpr<KeyT, ValueT> result(t_a);
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const AffExpr<KeyT, ValueT>& t_a, const QuadExpr<KeyT, ValueT>& t_b) {
        QuadExpr<KeyT, ValueT> result(t_b);
        result += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(QuadExpr<KeyT, ValueT>&& t_a, const QuadExpr<KeyT, ValueT>& t_b) {
        QuadExpr<KeyT, ValueT> result(std::move(t_a));
        result += t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const QuadExpr<KeyT, ValueT>& t_a, QuadExpr<KeyT, ValueT>&& t_b) {
        QuadExpr<KeyT, ValueT> result(std::move(t_b));
        result += t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    QuadExpr<KeyT, ValueT> operator+(const QuadExpr<KeyT, ValueT>& t_a, const QuadExpr<KeyT, ValueT>& t_b) {
        QuadExpr<KeyT, ValueT> result(t_a);
        result += t_b;
        return result;
    }

    // Subtraction

    // LinExpr

    template<class KeyT>
    LinExpr<KeyT> operator-(const KeyT& t_key) {
        return { -1, t_key};
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(const LinExpr<KeyT, ValueT>& t_lin_expr) {
        LinExpr<KeyT, ValueT> result;
        result -= t_lin_expr;
        return result;
    }

    template<class KeyT>
    LinExpr<KeyT> operator-(const KeyT& t_a, const KeyT& t_b) {
        LinExpr<KeyT> result(t_a);
        result -= LinExpr<KeyT>(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(LinExpr<KeyT, ValueT>&& t_a, const KeyT& t_b) {
        LinExpr<KeyT, ValueT> result(std::move(t_a));
        result -= LinExpr<KeyT, ValueT>(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(const KeyT& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(const LinExpr<KeyT, ValueT>& t_a, const KeyT& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result -= LinExpr<KeyT, ValueT>(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(const KeyT& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(LinExpr<KeyT, ValueT>&& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        LinExpr<KeyT, ValueT> result(std::move(t_a));
        result -= LinExpr<KeyT, ValueT>(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(const LinExpr<KeyT, ValueT>& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(LinExpr<KeyT, ValueT>&& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        LinExpr<KeyT, ValueT> result(std::move(t_a));
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    LinExpr<KeyT, ValueT> operator-(const LinExpr<KeyT, ValueT>& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        LinExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    // AffExpr

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(AffExpr<KeyT, ValueT>&& t_a, double t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result.constant() -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(double t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_b));
        result.constant() = t_a - result.constant();
        return result;
    }

    template<class KeyT>
    AffExpr<KeyT> operator-(double t_a, const KeyT& t_b) {
        AffExpr<KeyT> result(t_b);
        result.constant() = t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const AffExpr<KeyT, ValueT>& t_a, double t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result.constant() -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(double t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_b);
        result.constant() = t_a;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const KeyT& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(AffExpr<KeyT, ValueT>&& t_a, const KeyT& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const AffExpr<KeyT, ValueT>& t_a, const KeyT& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const KeyT& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(LinExpr<KeyT, ValueT>&& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const AffExpr<KeyT, ValueT>& t_a, LinExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(LinExpr<KeyT, ValueT>&& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const LinExpr<KeyT, ValueT>& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const AffExpr<KeyT, ValueT>& t_a, const LinExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const LinExpr<KeyT, ValueT>& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const AffExpr<KeyT, ValueT>& t_a, AffExpr<KeyT, ValueT>&& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= std::move(t_b);
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(AffExpr<KeyT, ValueT>&& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(std::move(t_a));
        result -= t_b;
        return result;
    }

    template<class KeyT, class ValueT>
    AffExpr<KeyT, ValueT> operator-(const AffExpr<KeyT, ValueT>& t_a, const AffExpr<KeyT, ValueT>& t_b) {
        AffExpr<KeyT, ValueT> result(t_a);
        result -= t_b;
        return result;
    }

    // QuadExpr

    // TODO
}

#endif //IDOL_OPERATORS_H
