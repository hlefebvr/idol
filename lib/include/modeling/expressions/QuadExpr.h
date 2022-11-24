//
// Created by henri on 21/11/22.
//

#ifndef IDOL_QUADEXPR_H
#define IDOL_QUADEXPR_H

#include "AbstractExpr.h"

template<class Key1 = Var, class Key2 = Key1>
struct QuadTerm {
    const Key1& key1;
    const Key2& key2;
    const Constant& constant;
    QuadTerm(const std::pair<Key1, Key2>& t_vars, const Constant& t_constant)
        : key1(t_vars.first), key2(t_vars.second), constant(t_constant) {}
};

template<class Key1 = Var,
        class Key2 = Key1,
        class Hash = std::conditional_t<
                std::is_same_v<Key1, Key2>,
                impl::symmetric_pair_hash,
                std::hash<std::pair<Key1, Key2>>
            >,
        class EqualTo = std::conditional_t<
                std::is_same_v<Key1, Key2>,
                impl::symmetric_pair_equal_to,
                std::equal_to<std::pair<Key1, Key2>>
            >
        >
class QuadExpr : public AbstractExpr<std::pair<Key1, Key2>, QuadTerm<Key1, Key2>, Hash, EqualTo> {
    friend class Matrix;

    using ParentT = AbstractExpr<std::pair<Key1, Key2>, QuadTerm<Key1, Key2>, Hash, EqualTo>;
    using ParentT::get;
    using ParentT::set;
public:
    QuadExpr() = default;
    QuadExpr(const Key1& t_a, const Key2& t_b);
    QuadExpr(Constant&& t_factor, const Key1& t_a, const Key2& t_b);
    QuadExpr(const Constant& t_factor, const Key1& t_a, const Key2& t_b);

    QuadExpr(const QuadExpr&) = default;
    QuadExpr(QuadExpr&&) noexcept = default;

    QuadExpr& operator=(const QuadExpr&) = default;
    QuadExpr& operator=(QuadExpr&&) noexcept = default;

    virtual ~QuadExpr() = default;

    void set(const Key1& t_a, const Key2& t_b, Constant t_coefficient);

    const Constant& get(const Key1& t_a, const Key2& t_b) const;
};

template<class Key1, class Key2, class Hash, class EqualTo>
QuadExpr<Key1, Key2, Hash, EqualTo>::QuadExpr(const Key1 &t_a, const Key2 &t_b) {
    set(t_a, t_b, 1);
}

template<class Key1, class Key2, class Hash, class EqualTo>
QuadExpr<Key1, Key2, Hash, EqualTo>::QuadExpr(Constant &&t_factor, const Key1 &t_a, const Key2 &t_b) {
    set(t_a, t_b, std::move(t_factor));
}

template<class Key1, class Key2, class Hash, class EqualTo>
QuadExpr<Key1, Key2, Hash, EqualTo>::QuadExpr(const Constant &t_factor, const Key1 &t_a, const Key2 &t_b) {
    set(t_a, t_b, Constant(t_factor));
}

template<class Key1, class Key2, class Hash, class EqualTo>
void QuadExpr<Key1, Key2, Hash, EqualTo>::set(const Key1 &t_a, const Key2& t_b, Constant t_coefficient) {
    return set(std::make_pair(t_a, t_b), std::move(t_coefficient));
}

template<class Key1, class Key2, class Hash, class EqualTo>
const Constant &QuadExpr<Key1, Key2, Hash, EqualTo>::get(const Key1 &t_a, const Key2 &t_b) const {
    return get({ t_a, t_b });
}

template<class Key1, class Key2>
std::ostream &operator<<(std::ostream& t_os, const QuadTerm<Key1, Key2>& t_term) {
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
