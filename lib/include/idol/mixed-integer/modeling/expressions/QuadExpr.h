//
// Created by henri on 21/11/22.
//

#ifndef IDOL_QUADEXPR_H
#define IDOL_QUADEXPR_H

#include "idol/general/utils/Pair.h"

namespace idol {
    template<class Key1, class Key2>
    class QuadExpr;
}

template<class Key1 = idol::Var, class Key2 = Key1>
class idol::QuadExpr : public SparseVector<idol::Pair<Key1, Key2>, double> {
public:
    QuadExpr() = default;
    QuadExpr(const Key1& t_a, const Key2& t_b);
    QuadExpr(double t_factor, const Key1& t_a, const Key2& t_b);

    QuadExpr(const QuadExpr&) = default;
    QuadExpr(QuadExpr&&) noexcept = default;

    QuadExpr& operator=(const QuadExpr&) = default;
    QuadExpr& operator=(QuadExpr&&) noexcept = default;

    virtual ~QuadExpr() = default;

    void set(const Key1& t_a, const Key2& t_b, double t_coefficient);

    double get(const Key1& t_a, const Key2& t_b) const;
};

template<class Key1, class Key2>
idol::QuadExpr<Key1, Key2>::QuadExpr(const Key1 &t_a, const Key2 &t_b)
    : SparseVector<idol::Pair<Key1, Key2>, double>(
            { {t_a, t_b} },
            { 1. },
            SparseVector<idol::Pair<Key1, Key2>, double>::SortingCriteria::Index,
            true) {

}

template<class Key1, class Key2>
idol::QuadExpr<Key1, Key2>::QuadExpr(double t_factor, const Key1 &t_a, const Key2 &t_b)
    : SparseVector<idol::Pair<Key1, Key2>, double>(
            { {t_a, t_b} },
            { t_factor },
            SparseVector<idol::Pair<Key1, Key2>, double>::SortingCriteria::Index,
            true
            ) {

}

template<class Key1, class Key2>
void idol::QuadExpr<Key1, Key2>::set(const Key1 &t_a, const Key2& t_b, double t_coefficient) {
    return SparseVector<idol::Pair<Key1, Key2>, double>::set({ t_a, t_b }, std::move(t_coefficient));
}

template<class Key1, class Key2>
double idol::QuadExpr<Key1, Key2>::get(const Key1 &t_a, const Key2 &t_b) const {
    return SparseVector<idol::Pair<Key1, Key2>, double>::get({ t_a, t_b });
}

#endif //IDOL_QUADEXPR_H
