//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_H
#define IDOL_OPERATORS_H

#include "operators_utils.h"
#include "idol/general/utils/Point.h"
#include <idol/mixed-integer/modeling/expressions/QuadExpr.h>
#include <idol/mixed-integer/modeling/variables/Var.h>
#include <idol/mixed-integer/modeling/constraints/Ctr.h>

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

#define DEFINE_OPERATOR_SINGLE(OPERATOR, OPERATOR_EQ, T, U, ResultT) \
static ResultT operator OPERATOR(T t_a, U t_b) { \
    ResultT result(std::move(t_a));                                   \
    result OPERATOR_EQ std::move(t_b);                                \
    return result; \
}

#define DEFINE_OPERATOR(OPERATOR, OPERATOR_EQ, T, U, ResultT) \
DEFINE_OPERATOR_SINGLE(OPERATOR, OPERATOR_EQ, T, U, ResultT) \
static ResultT operator OPERATOR(U t_a, T t_b) {             \
    ResultT result(std::move(t_a));          \
    result OPERATOR_EQ std::move(t_b);                 \
    return result;                            \
}

#define DEFINE_COMMUTATIVE_OPERATOR_SINGLE(OPERATOR, OPERATOR_EQ, T, U, ResultT) \
static ResultT operator OPERATOR(T t_a, U t_b) { \
    ResultT result(std::move(t_b));                                   \
    result OPERATOR_EQ std::move(t_a);                                \
    return result; \
}

#define DEFINE_COMMUTATIVE_OPERATOR(OPERATOR, OPERATOR_EQ, T, U, ResultT) \
DEFINE_COMMUTATIVE_OPERATOR_SINGLE(OPERATOR, OPERATOR_EQ, T, U, ResultT) \
static ResultT operator OPERATOR(U t_a, T t_b) {             \
    ResultT result(std::move(t_a));          \
    result OPERATOR_EQ std::move(t_b);                 \
    return result;                            \
}

#define DEFINE_OPERATIONS(T) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, double, LinExpr<T>, AffExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, double, AffExpr<T>, AffExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, double, QuadExpr<T>, QuadExpr<T>) \
 \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, T, double, AffExpr<T>)          \
DEFINE_COMMUTATIVE_OPERATOR_SINGLE(+, +=, T, T, LinExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, T, LinExpr<T>, LinExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, T, AffExpr<T>, AffExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, T, QuadExpr<T>, QuadExpr<T>) \
 \
DEFINE_COMMUTATIVE_OPERATOR_SINGLE(+, +=, LinExpr<T>, LinExpr<T>, LinExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, LinExpr<T>, AffExpr<T>, AffExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, LinExpr<T>, QuadExpr<T>, QuadExpr<T>) \
 \
DEFINE_COMMUTATIVE_OPERATOR(+, +=, AffExpr<T>, QuadExpr<T>, QuadExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR_SINGLE(+, +=, AffExpr<T>, AffExpr<T>, AffExpr<T>) \
 \
DEFINE_COMMUTATIVE_OPERATOR_SINGLE(+, +=, QuadExpr<T>, QuadExpr<T>, QuadExpr<T>)    \
                             \
DEFINE_OPERATOR(-, -=, double, T, AffExpr<T>)                             \
DEFINE_OPERATOR(-, -=, double, LinExpr<T>, AffExpr<T>)                             \
DEFINE_OPERATOR(-, -=, double, AffExpr<T>, AffExpr<T>)             \
DEFINE_OPERATOR(-, -=, double, QuadExpr<T>, QuadExpr<T>)             \
                             \
DEFINE_OPERATOR_SINGLE(-, -=, T, T, LinExpr<T>)                             \
DEFINE_OPERATOR(-, -=, T, LinExpr<T>, LinExpr<T>)                             \
DEFINE_OPERATOR(-, -=, T, AffExpr<T>, AffExpr<T>)                             \
DEFINE_OPERATOR(-, -=, T, QuadExpr<T>, QuadExpr<T>)                             \
                             \
DEFINE_OPERATOR_SINGLE(-, -=, LinExpr<T>, LinExpr<T>, LinExpr<T>)                             \
DEFINE_OPERATOR(-, -=, LinExpr<T>, AffExpr<T>, AffExpr<T>)                             \
DEFINE_OPERATOR(-, -=, LinExpr<T>, QuadExpr<T>, QuadExpr<T>)                             \
                             \
DEFINE_OPERATOR_SINGLE(-, -=, AffExpr<T>, AffExpr<T>, AffExpr<T>)                             \
DEFINE_OPERATOR(-, -=, AffExpr<T>, QuadExpr<T>, QuadExpr<T>)       \
                             \
DEFINE_OPERATOR_SINGLE(-, -=, QuadExpr<T>, QuadExpr<T>, QuadExpr<T>)                             \
                             \
DEFINE_COMMUTATIVE_OPERATOR(*, *=, double, T, LinExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(*, *=, double, LinExpr<T>, LinExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(*, *=, double, AffExpr<T>, AffExpr<T>) \
DEFINE_COMMUTATIVE_OPERATOR(*, *=, double, QuadExpr<T>, QuadExpr<T>) \
\
static QuadExpr<T> operator*(T t_a, T t_b) { return { t_a, t_b }; }   \
\
static LinExpr<T, double> operator+(const T& t_a) { return t_a; } \
static LinExpr<T, double> operator-(const T& t_a) { return { -1., t_a }; }    \
static QuadExpr<T> operator*(const T& t_a, const LinExpr<T>& t_b) {           \
    QuadExpr<T> result;      \
    for (const auto& [var, constant] : t_b) {                      \
        result += constant * (var * t_a);\
    }                        \
    return result;           \
}                            \
static QuadExpr<T> operator*(const LinExpr<T>& t_a, const T& t_b) { return t_b * t_a; }       \
static QuadExpr<T> operator*(const T& t_a, const AffExpr<T>& t_b) { return t_a * t_b.constant() + t_a * t_b.linear(); } \
static QuadExpr<T> operator*(const AffExpr<T>& t_a, const T& t_b) { return t_b * t_a; }       \
static LinExpr<T> operator/(const T& t_a, double t_b) { return (1./t_b) * t_a; }    \
static LinExpr<T> operator/(const LinExpr<T>& t_a, double t_b) { return (1./t_b) * t_a; } \
static AffExpr<T> operator/(const AffExpr<T>& t_a, double t_b) { return (1./t_b) * t_a; } \
static QuadExpr<T> operator/(const QuadExpr<T>& t_a, double t_b) { return (1./t_b) * t_a; }

namespace idol {
    DEFINE_OPERATIONS(Var)
    DEFINE_OPERATIONS(Ctr)
}

#endif //IDOL_OPERATORS_H
