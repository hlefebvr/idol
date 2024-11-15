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
 * @tparam Key the class representing keys
 */
template<class Key = idol::Var, class ValueT = double>
class idol::LinExpr : public SparseVector<Key, ValueT> {
public:
    LinExpr() = default;
    LinExpr(SparseVector<Key, ValueT>&& t_vector) : SparseVector<Key, ValueT>(std::move(t_vector)) {}
    LinExpr(const Key& t_key); // NOLINT(google-explicit-constructor)
    LinExpr(const ValueT& t_factor, const Key& t_key);
    LinExpr(ValueT&& t_factor, const Key& t_key);
};

template<class Key, class ValueT>
idol::LinExpr<Key, ValueT>::LinExpr(ValueT &&t_factor, const Key &t_key) : SparseVector<Key, ValueT>(t_key, std::move(t_factor)) {

}

template<class Key, class ValueT>
idol::LinExpr<Key, ValueT>::LinExpr(const Key &t_key) : SparseVector<Key, double>(t_key, 1.) {

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
            t_os << it->second << " " << it->first;
        } else {
            t_os << "[ " << it->second << " ] " << it->first;
        }
        ++it;
        for (const auto end = t_expr.end() ; it != end ; ++it) {
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
