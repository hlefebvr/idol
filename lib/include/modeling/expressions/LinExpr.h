//
// Created by henri on 03/10/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "../../containers/Map.h"
#include "../../containers/IteratorForward.h"
#include "../matrix/AbstractMatrixCoefficient.h"
#include "Constant.h"
#include "./operations/operators_Constant.h"
#include "../matrix/MatrixCoefficient.h"
#include "../variables/Var.h"
#include "../../errors/Exception.h"
#include "AbstractExpr.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

template<class Key = Var>
struct LinTerm {
    const Key& key;
    const Constant& constant;
    LinTerm(const Key& t_key, const Constant& t_constant)
        : key(t_key), constant(t_constant) {}
};

/**
 * @tparam Key the class representing keys
 */
template<class Key = Var>
class LinExpr : public AbstractExpr<Key, LinTerm<Key>> {
    friend class Matrix;
public:
    LinExpr() = default;
    LinExpr(const Key& t_key); // NOLINT(google-explicit-constructor)
    LinExpr(Constant&& t_factor, const Key& t_key);
    LinExpr(const Constant& t_factor, const Key& t_key);

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
    void set(const Key& t_key, const Constant& t_coefficient);

    /**
     * Sets the value of a key's coefficient
     *
     * The constant is moved.
     *
     * Example:
     * ```cpp
     * LinExpr lin_expr;
     * lin_expr.set(x[0], -1);
     * ```
     * @param t_key the key
     * @param t_coefficient the key's coefficient
     */
    void set(const Key& t_key, Constant&& t_coefficient) override;

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
    const Constant& get(const Key& t_key) const override;

    /**
     * Sets the coefficient of a key to zero
     *
     * This is equivalent to calling `LinExpr<Key>::set` with a zero coefficient.
     * @param t_key the key
     */
    void remove(const Key& t_key) override;

};

template<class Key>
void LinExpr<Key>::remove(const Key &t_key) {
    AbstractExpr<Key, LinTerm<Key>>::remove(t_key);
}

template<class Key>
const Constant &LinExpr<Key>::get(const Key &t_key) const {
    return AbstractExpr<Key, LinTerm<Key>>::get(t_key);
}

template<class Key>
void LinExpr<Key>::set(const Key &t_key, Constant &&t_coefficient) {
    AbstractExpr<Key, LinTerm<Key>>::set(t_key, std::move(t_coefficient));
}

template<class Key>
LinExpr<Key>::LinExpr(const Key &t_key) {
    set(t_key, 1.);
}

template<class Key>
LinExpr<Key>::LinExpr(Constant&& t_factor, const Key &t_key) {
    set(t_key, std::move(t_factor));
}

template<class Key>
LinExpr<Key>::LinExpr(const Constant& t_factor, const Key &t_key) {
    set(t_key, Constant(t_factor));
}

template<class Key>
void LinExpr<Key>::set(const Key &t_key, const Constant& t_coefficient) {
    AbstractExpr<Key, LinTerm<Key>>::set(t_key, Constant(t_coefficient));
}

template<class Key>
std::ostream& operator<<(std::ostream& t_os, const LinTerm<Key>& t_term) {

    if (t_term.constant.is_zero()) {
        return t_os << "0";
    }

    if (t_term.constant.is_numerical()) {
        t_os << t_term.constant.numerical();
    } else {
        t_os << '(' << t_term.constant << ')';
    }

    return t_os << ' ' << t_term.key;
}

#endif //OPTIMIZE_EXPR_H
