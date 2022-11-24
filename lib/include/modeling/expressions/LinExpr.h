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

template<class Key = Var>
class LinExpr : public AbstractExpr<Key, LinTerm<Key>> {
    friend class Matrix;
public:
    LinExpr() = default;
    LinExpr(const Key& t_key); // NOLINT(google-explicit-constructor)
    LinExpr(Constant&& t_factor, const Key& t_key);
    LinExpr(const Constant& t_factor, const Key& t_key);

    bool set(const Key& t_key, const Constant& t_coefficient);

    using AbstractExpr<Key, LinTerm<Key>>::get;
    using AbstractExpr<Key, LinTerm<Key>>::set;
    using AbstractExpr<Key, LinTerm<Key>>::remove;

    /**
     * Replaces every a Key with the returned expression (if any) by t_function.
     *
     * Can be used to substitute, for instance, a Var by an Expr (e.g., \f$ x = \sum_e \alpha_e\textbf x^e \f$).
     * @param t_function A function which returns an optional expression to replace its argument.
     */
    void replace_if(const std::function<std::optional<LinExpr<Key>>(const Key&)>& t_function);

};

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
bool LinExpr<Key>::set(const Key &t_key, const Constant& t_coefficient) {
    return AbstractExpr<Key, LinTerm<Key>>::set(t_key, Constant(t_coefficient));
}

template<class Key>
void LinExpr<Key>::replace_if(const std::function<std::optional<LinExpr<Key>>(const Key &)> &t_function) {

    std::list<LinExpr<Key>> to_add;

    auto it = this->refs().begin();
    const auto end = this->refs().end();

    while (it != end) {

        const auto& [key, ref] = *it;

        if (auto result = t_function(key) ; result.has_value()) {

            auto expr = ref.value().numerical() * result.value();
            to_add.template emplace_back(std::move(expr));
            it = this->refs().erase(it);

        } else {
            ++it;
        }

    }

    for (auto& expr : to_add) {
        *this += expr;
    }
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
