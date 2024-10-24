//
// Created by henri on 03/10/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "idol/general/utils/SparseVector.h"
#include "idol/general/utils/IteratorForward.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/general/utils/exceptions/Exception.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

namespace idol {
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
    LinExpr(const Key& t_key); // NOLINT(google-explicit-constructor)
    LinExpr(double t_factor, const Key& t_key);
};

template<class Key>
idol::LinExpr<Key>::LinExpr(const Key &t_key) : SparseVector<Key, double>({ t_key }, { 1. }, SparseVector<Key, double>::SortingCriteria::Index, true) {

}

template<class Key>
idol::LinExpr<Key>::LinExpr(double t_factor, const Key &t_key) : SparseVector<Key, double>({ t_key }, { t_factor }, SparseVector<Key, double>::SortingCriteria::Index, true) {
}

#endif //OPTIMIZE_EXPR_H
