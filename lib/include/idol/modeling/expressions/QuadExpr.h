//
// Created by henri on 21/11/22.
//

#ifndef IDOL_QUADEXPR_H
#define IDOL_QUADEXPR_H

#include "AbstractExpr.h"
#include "idol/utils/Pair.h"

namespace idol {
    template<class Key1, class Key2>
    struct QuadTerm;

    template<class Key1,
            class Key2,
            class Hash,
            class EqualTo
    >
    class QuadExpr;
}

template<class Key1 = idol::Var, class Key2 = Key1>
struct idol::QuadTerm {
    const Key1& key1;
    const Key2& key2;
    const double constant;
    QuadTerm(const Pair<Key1, Key2>& t_vars, double t_constant)
        : key1(t_vars.first), key2(t_vars.second), constant(t_constant) {}
};

template<class Key1 = idol::Var,
        class Key2 = Key1,
        class Hash = std::conditional_t<
                std::is_same_v<Key1, Key2>,
                idol::impl::symmetric_pair_hash,
                std::hash<idol::Pair<Key1, Key2>>
            >,
        class EqualTo = std::conditional_t<
                std::is_same_v<Key1, Key2>,
                idol::impl::symmetric_pair_equal_to,
                std::equal_to<idol::Pair<Key1, Key2>>
            >
        >
class idol::QuadExpr : public AbstractExpr<idol::Pair<Key1, Key2>, QuadTerm<Key1, Key2>, Hash, EqualTo> {
    friend class Matrix;

    using ParentT = AbstractExpr<idol::Pair<Key1, Key2>, QuadTerm<Key1, Key2>, Hash, EqualTo>;
    using ParentT::get;
    using ParentT::set;
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

template<class Key1, class Key2, class Hash, class EqualTo>
idol::QuadExpr<Key1, Key2, Hash, EqualTo>::QuadExpr(const Key1 &t_a, const Key2 &t_b) {
    set(t_a, t_b, 1);
}

template<class Key1, class Key2, class Hash, class EqualTo>
idol::QuadExpr<Key1, Key2, Hash, EqualTo>::QuadExpr(double t_factor, const Key1 &t_a, const Key2 &t_b) {
    set(t_a, t_b, t_factor);
}

template<class Key1, class Key2, class Hash, class EqualTo>
void idol::QuadExpr<Key1, Key2, Hash, EqualTo>::set(const Key1 &t_a, const Key2& t_b, double t_coefficient) {
    return set({ t_a, t_b }, std::move(t_coefficient));
}

template<class Key1, class Key2, class Hash, class EqualTo>
double idol::QuadExpr<Key1, Key2, Hash, EqualTo>::get(const Key1 &t_a, const Key2 &t_b) const {
    return get({ t_a, t_b });
}

namespace idol {

    template<class Key1, class Key2>
    std::ostream &operator<<(std::ostream &t_os, const QuadTerm<Key1, Key2> &t_term) {
        if (std::abs(t_term.constant) < Tolerance::Sparsity) {
            return t_os << "0";
        }

        return t_os << t_term.constant << ' ' << t_term.key1 << ' ' << t_term.key2;
    }

}

#endif //IDOL_QUADEXPR_H
