//
// Created by henri on 21/11/22.
//

#ifndef IDOL_QUADEXPR_H
#define IDOL_QUADEXPR_H

#include "AbstractExpr.h"

template<class Key = Var>
struct QuadTerm {
    const Key& key1;
    const Key& key2;
    const Constant& constant;
    QuadTerm(const std::pair<Key, Key>& t_vars, const Constant& t_constant)
        : key1(t_vars.first), key2(t_vars.second), constant(t_constant) {}
};

template<class Key = Var>
class QuadExpr : public AbstractExpr<std::pair<Key, Key>, QuadTerm<Key>> {
    friend class Matrix;

    using AbstractExpr<std::pair<Key, Key>, QuadTerm<Key>>::get;
    using AbstractExpr<std::pair<Key, Key>, QuadTerm<Key>>::set;
public:
    QuadExpr() = default;
    QuadExpr(const Key& t_a, const Key& t_b);
    QuadExpr(Constant&& t_factor, const Key& t_a, const Key& t_b);
    QuadExpr(const Constant& t_factor, const Key& t_a, const Key& t_b);

    QuadExpr(const QuadExpr&) = default;
    QuadExpr(QuadExpr&&) noexcept = default;

    QuadExpr& operator=(const QuadExpr&) = default;
    QuadExpr& operator=(QuadExpr&&) noexcept = default;

    virtual ~QuadExpr() = default;

    bool set(const Key& t_a, const Key& t_b, Constant t_coefficient);

    const Constant& get(const Key& t_a, const Key& t_b) const;
};

template<class Key>
QuadExpr<Key>::QuadExpr(const Key &t_a, const Key &t_b) {
    set(t_a, t_b, 1);
}

template<class Key>
QuadExpr<Key>::QuadExpr(Constant &&t_factor, const Key &t_a, const Key &t_b) {
    set(t_a, t_b, std::move(t_factor));
}

template<class Key>
QuadExpr<Key>::QuadExpr(const Constant &t_factor, const Key &t_a, const Key &t_b) {
    set(t_a, t_b, Constant(t_factor));
}

template<class Key>
bool QuadExpr<Key>::set(const Key &t_a, const Key& t_b, Constant t_coefficient) {
    return set(std::make_pair(t_a, t_b), std::move(t_coefficient));
}

template<class Key>
const Constant &QuadExpr<Key>::get(const Key &t_a, const Key &t_b) const {
    return get({ t_a, t_b });
}

template<class Key>
std::ostream &operator<<(std::ostream& t_os, const QuadTerm<Key>& t_term) {
    if (t_term.constant.is_zero()) {
        return t_os << "0";
    }

    if (t_term.constant.is_numerical()) {
        t_os << t_term.constant.numerical();
    } else {
        t_os << '(' << t_term.constant << ')';
    }

    return t_os << ' ' << t_term.key1 << ' ' << t_term.key2;
}

#endif //IDOL_QUADEXPR_H
