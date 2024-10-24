//
// Created by henri on 03/10/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "idol/general/utils/Map.h"
#include "idol/general/utils/IteratorForward.h"
#include "idol/mixed-integer/modeling/matrix/AbstractMatrixCoefficient.h"
#include "Constant.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators_Constant.h"
#include "idol/mixed-integer/modeling/matrix/MatrixCoefficient.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "AbstractExpr.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

namespace idol {
    template<class Key>
    struct LinTerm;

    template<class Key>
    class LinExpr;
}

template<class Key = idol::Var>
struct idol::LinTerm {
    const Key& key;
    const double constant;
    LinTerm(const Key& t_key, double t_constant)
        : key(t_key), constant(t_constant) {}
};

/**
 * @tparam Key the class representing keys
 */
template<class Key = idol::Var>
class idol::LinExpr : public AbstractExpr<Key, LinTerm<Key>> {
    friend class Matrix;
public:
    LinExpr() = default;
    LinExpr(const Key& t_key); // NOLINT(google-explicit-constructor)
    LinExpr(double t_factor, const Key& t_key);

    /**
     * Sets the value of a key's coefficient
     *
     * The constant is copied.
     *
     * Example:
     * ```cpp
     * LinExpr lin_expr;
     * lin_expr.set(x[0], -1);
     * ```
     * @param t_key the key
     * @param t_coefficient the key's coefficient
     */
    void set(const Key& t_key, double t_coefficient);

    /**
     * Returns a key's coefficient
     *
     * Example:
     * ```cpp
     * LinExpr lin_expr = 2 * x[0] + 3 * x[1];
     *
     * std::cout << lin_expr.get(x[0]) << std::endl; // output: 2
     * ```
     * @param t_key the key
     * @return the key's coefficient
     */
    double get(const Key& t_key) const override;

    /**
     * Sets the coefficient of a key to zero
     *
     * This is equivalent to calling `LinExpr<Key>::set` with a zero coefficient.
     * @param t_key the key
     */
    void remove(const Key& t_key) override;

    LinExpr fix(const PrimalPoint& t_primals) const;
};

template<class Key>
idol::LinExpr<Key> idol::LinExpr<Key>::fix(const idol::PrimalPoint &t_primals) const {
    auto result = *this;
    result.internal_fix(t_primals);
    return result;
}

template<class Key>
void idol::LinExpr<Key>::remove(const Key &t_key) {
    AbstractExpr<Key, LinTerm<Key>>::remove(t_key);
}

template<class Key>
double idol::LinExpr<Key>::get(const Key &t_key) const {
    return AbstractExpr<Key, LinTerm<Key>>::get(t_key);
}

template<class Key>
idol::LinExpr<Key>::LinExpr(const Key &t_key) {
    set(t_key, 1.);
}

template<class Key>
idol::LinExpr<Key>::LinExpr(double t_factor, const Key &t_key) {
    set(t_key, std::move(t_factor));
}

template<class Key>
void idol::LinExpr<Key>::set(const Key &t_key, double t_coefficient) {
    AbstractExpr<Key, LinTerm<Key>>::set(t_key, t_coefficient);
}

namespace idol {

    template<class Key>
    std::ostream &operator<<(std::ostream &t_os, const LinTerm<Key> &t_term) {

        if (std::abs(t_term.constant) < Tolerance::Sparsity) {
            return t_os << "0";
        }

        return t_os << t_term.constant << ' ' << t_term.key;
    }

}

#endif //OPTIMIZE_EXPR_H
