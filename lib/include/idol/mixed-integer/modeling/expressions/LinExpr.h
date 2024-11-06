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

    template<class Key>
    class LinExpr;
}

/**
 * @tparam Key the class representing keys
 */
template<class Key = idol::Var>
class idol::LinExpr : public SparseVector<Key, double> {
public:
    LinExpr() = default;
    LinExpr(SparseVector<Key, double>&& t_vector) : SparseVector<Key, double>(std::move(t_vector)) {}
    LinExpr(const Key& t_key); // NOLINT(google-explicit-constructor)
    LinExpr(double t_factor, const Key& t_key);
};

template<class Key>
idol::LinExpr<Key>::LinExpr(const Key &t_key) : SparseVector<Key, double>({ t_key }, { 1. }, SparseVector<Key, double>::SortingCriteria::Index, true) {

}

template<class Key>
idol::LinExpr<Key>::LinExpr(double t_factor, const Key &t_key) : SparseVector<Key, double>({ t_key }, { t_factor }, SparseVector<Key, double>::SortingCriteria::Index, true) {
}

namespace idol {
    template<class Key>
    std::ostream& operator<<(std::ostream& t_os, const LinExpr<Key>& t_expr) {

        if (t_expr.empty()) {
            return t_os << "0";
        }

        unsigned int i = 0;
        t_os << t_expr.value_at(i) << " " << t_expr.index_at(i);
        ++i;
        for (unsigned int n = t_expr.size() ; i < n ; ++i) {
            t_os << " + " << t_expr.value_at(i) << " " << t_expr.index_at(i);
        }

        return t_os;
    }
}

#endif //OPTIMIZE_EXPR_H
