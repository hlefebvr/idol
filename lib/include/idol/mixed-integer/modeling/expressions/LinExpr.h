//
// Created by henri on 03/10/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "idol/general/utils/SparseVector.h"
#include "idol/general/utils/IteratorForward.h"
#include "idol/general/utils/exceptions/Exception.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

namespace idol {
    class Var;

    template<class KeyT, class ValueT>
    class LinExpr;
}

/**
 * @tparam KeyT the class representing keys
 */
template<class KeyT = idol::Var, class ValueT = double>
class idol::LinExpr : public SparseVector<KeyT, ValueT> {
public:
    LinExpr() = default;
    LinExpr(KeyT t_key); // NOLINT(google-explicit-constructor)
    LinExpr(SparseVector<KeyT, ValueT> t_vector) : SparseVector<KeyT, ValueT>(std::move(t_vector)) {} // NOLINT(*-explicit-constructor)
    LinExpr(const ValueT& t_factor, const KeyT& t_key);
    LinExpr(ValueT&& t_factor, const KeyT& t_key);

    LinExpr(const LinExpr<KeyT, ValueT>&) = default;
    LinExpr(LinExpr<KeyT, ValueT>&&) = default;

    LinExpr& operator=(const LinExpr<KeyT, ValueT>&) noexcept = default;
    LinExpr& operator=(LinExpr<KeyT, ValueT>&&) noexcept = default;

    LinExpr& operator+=(const LinExpr<KeyT, ValueT>& t_rhs);
    LinExpr& operator+=(const KeyT& t_rhs);

    LinExpr& operator-=(const LinExpr<KeyT, ValueT>& t_rhs);
    LinExpr& operator-=(const KeyT& t_rhs);

    static LinExpr<KeyT, ValueT> Zero;
};

template<class KeyT, class ValueT> idol::LinExpr<KeyT, ValueT> idol::LinExpr<KeyT, ValueT>::Zero {};

template<class KeyT, class ValueT>
idol::LinExpr<KeyT, ValueT> &idol::LinExpr<KeyT, ValueT>::operator-=(const KeyT &t_rhs) {
    SparseVector<KeyT, ValueT>::operator-=(SparseVector<KeyT, ValueT>(t_rhs, 1));
    return *this;
}

template<class KeyT, class ValueT>
idol::LinExpr<KeyT, ValueT> &idol::LinExpr<KeyT, ValueT>::operator-=(const idol::LinExpr<KeyT, ValueT> &t_rhs) {
    SparseVector<KeyT, ValueT>::operator-=((SparseVector<KeyT, ValueT>&) t_rhs);
    return *this;
}

template<class KeyT, class ValueT>
idol::LinExpr<KeyT, ValueT> &idol::LinExpr<KeyT, ValueT>::operator+=(const KeyT &t_rhs) {
    SparseVector<KeyT, ValueT>::operator+=(SparseVector<KeyT, ValueT>(t_rhs, 1));
    return *this;
}

template<class Key, class ValueT>
idol::LinExpr<Key, ValueT>& idol::LinExpr<Key, ValueT>::operator+=(const idol::LinExpr<Key, ValueT> &t_rhs) {
    SparseVector<Key, ValueT>::operator+=((SparseVector<Key, ValueT>&) t_rhs);
    return *this;
}

template<class Key, class ValueT>
idol::LinExpr<Key, ValueT>::LinExpr(ValueT &&t_factor, const Key &t_key) : SparseVector<Key, ValueT>(t_key, std::move(t_factor)) {

}

template<class Key, class ValueT>
idol::LinExpr<Key, ValueT>::LinExpr(Key t_key) : SparseVector<Key, double>(t_key, 1.) {

}

template<class Key, class ValueT>
idol::LinExpr<Key, ValueT>::LinExpr(const ValueT& t_factor, const Key &t_key) : SparseVector<Key, ValueT>(t_key, t_factor) {
}

namespace idol {
    template<class KeyT, class ValueT>
    std::ostream& operator<<(std::ostream& t_os, const LinExpr<KeyT, ValueT>& t_expr) {

        if (t_expr.empty()) {
            return t_os << "0";
        }

        auto it = t_expr.begin();
        if constexpr (std::is_same_v<ValueT, double>) {
            t_os << it->second;
            t_os << " ";
            t_os << it->first;
        } else {
            t_os << "[ " << it->second << " ] " << it->first;
        }
        ++it;
        for (const auto end = t_expr.end() ; it != end ; ++it) {
            t_os << " + ";
            if constexpr (std::is_same_v<ValueT, double>) {
                t_os << it->second << " " << it->first;
            } else {
                t_os << "[ " << it->second << " ] " << it->first;
            }
        }

        return t_os;
    }
}

#endif //OPTIMIZE_EXPR_H
