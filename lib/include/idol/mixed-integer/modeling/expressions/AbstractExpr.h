//
// Created by henri on 21/11/22.
//

#ifndef IDOL_ABSTRACTEXPR_H
#define IDOL_ABSTRACTEXPR_H

#include "idol/general/utils/Map.h"
#include "idol/general/utils/IteratorForward.h"
#include "idol/mixed-integer/modeling/matrix/AbstractMatrixCoefficient.h"
#include "Constant.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators_Constant.h"
#include "idol/mixed-integer/modeling/matrix/MatrixCoefficient.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/general/utils/Pair.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

namespace idol {

    namespace impl {
        template<class, class, class, class>
        class AbstractExpr;
    }

    template<class Key,
            class IteratorOutputT,
            class Hash,
            class EqualTo
    >
    class AbstractExpr;
}

template<class Key,
        class IteratorOutputT,
        class Hash,
        class EqualTo>
class idol::impl::AbstractExpr {
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient>, Hash, EqualTo>;
    MapType m_map;
protected:
    virtual void set(const Key& t_key, double t_coefficient);
    virtual double get(const Key& t_key) const;
    virtual void remove(const Key& t_key);
    void internal_fix(const PrimalPoint& t_primals);

    class References;
    References refs();
public:
    AbstractExpr() = default;

    /**
     * Copy constructor.
     *
     * Each term are copied by value.
     *
     * @param t_src The source object to copy.
     */
    AbstractExpr(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_src);

    /**
     * Move constructor.
     *
     * Each term is moved.
     *
     * In particular, MatrixCoefficientReference-s remain MatrixCoefficientReference-s.
     *
     * @param t_src
     */
    AbstractExpr(AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>&& t_src) noexcept = default;

    /**
     * Assignment operator.
     *
     * Each term is copied by value.
     *
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator=(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_src);

    /**
     * Assignment move operator.
     *
     * Each term is moved.
     *
     * In particular, MatrixCoefficientReference-s remain MatrixCoefficientReference-s.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator=(AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>&& t_src) noexcept = default;

    /**
     * Adds each term of `t_rhs` to the expression.
     *
     * Zero entries are removed from the resulting expression.
     *
     * @param t_rhs the expression whose terms are to be added up.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator+=(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_rhs);

    /**
     * Subtracts each term of `t_rhs` to the expression.
     *
     * Zero entries are removed from the resulting expression.
     *
     * @param t_rhs The object whose terms are to be subtracted.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator-=(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_rhs);

    /**
     * Multiplies every term of the expression by `t_factor`.
     *
     * If `t_factor` equals zero with tolerance `Tolerance::Sparsity`, then the expression is emptied.
     *
     * @param t_factor The multiplying factor.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator*=(double t_factor);

    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator/=(double t_factor);

    /**
     * Returns the size of the expression (i.e., number of non-zero entries which are currently stored).
     *
     * @return the size of the expression
     */
    [[nodiscard]] unsigned int size() const { return m_map.size(); }

    /**
     * Returns true if and only if the size is zero.
     *
     * @return true if and only if the size is zero
     */
    [[nodiscard]] bool empty() const { return m_map.empty(); }

    class const_iterator;

    /**
     * Returns an iterator pointing to the first term of the expression.
     * @return an iterator pointing to the first term of the expression
     */
    const_iterator begin() const { return const_iterator(m_map.begin()); }

    /**
     * Returns an iterator pointing to the end of the expression.
     * @return an iterator pointing to the end the expression
     */
    const_iterator end() const { return const_iterator(m_map.end()); }

    /**
     * Removes all entries from the expression.
     */
    void clear() { m_map.clear(); }

    void round();

    AbstractExpr multiply_with_precision(double t_factor, unsigned int t_n_digits);

    AbstractExpr multiply_with_precision_by_power_of_10(unsigned int t_exponent, unsigned int t_n_digits);

    long gcd() const;
};

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
void idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::internal_fix(const idol::PrimalPoint &t_primals) {

    for (auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value() = ptr_to_value->value().fix(t_primals);
    }

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::multiply_with_precision_by_power_of_10(unsigned int t_exponent, unsigned int t_n_digits) {

    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value().multiply_with_precision_by_power_of_10(t_exponent, t_n_digits);
    }

    return *this;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::multiply_with_precision(double t_factor, unsigned int t_n_digits) {

    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value().multiply_with_precision(t_factor, t_n_digits);
    }

    return *this;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
long idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::gcd() const {
    long result = 0;
    for (const auto& [key, ptr_to_value] : m_map) {
        result = std::gcd(result, (long) ptr_to_value->value());
    }
    return result;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
void idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::round() {
    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value().round();
    }
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::AbstractExpr(const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_src) {

    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator=(const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_src) {

    if (this == &t_src) {
        return *this;
    }

    m_map.clear();
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }

    return *this;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator+=(const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_rhs) {

    for (const auto& [key, ptr_to_value] : t_rhs.m_map) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
        } else {
            it->second->value() += ptr_to_value->value();
            if (std::abs(it->second->value()) < Tolerance::Sparsity) {
                m_map.erase(it);
            }
        }
    }

    return *this;

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator-=(
        const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_rhs) {

    for (const auto& [key, ptr_to_value] : t_rhs.m_map) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_map.template emplace(key, std::make_unique<MatrixCoefficient>(-ptr_to_value->value()));
        } else {
            it->second->value() -= ptr_to_value->value();
            if (std::abs(it->second->value()) < Tolerance::Sparsity) {
                m_map.erase(it);
            }
        }
    }
    return *this;

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator*=(double t_factor) {

    if (equals(t_factor, 0., Tolerance::Sparsity)) {
        m_map.clear();
        return *this;
    }

    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value() *= t_factor;
    }

    return *this;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator/=(double t_factor) {

    if (equals(t_factor, 1., Tolerance::Sparsity)) {
        return *this;
    }

    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value() /= t_factor;
    }

    return *this;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
void idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::set(const Key &t_key, double t_coefficient) {

    if (std::abs(t_coefficient) < Tolerance::Sparsity) {
        m_map.erase(t_key);
        return;
    }

    auto it = m_map.find(t_key);
    if (it == m_map.end()) {
        m_map.emplace(t_key, std::make_unique<MatrixCoefficient>(t_coefficient));
        return;
    }

    it->second->set_value(t_coefficient);

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
double idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? 0 : it->second->value();
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
void idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::remove(const Key &t_key) {
    m_map.erase(t_key);
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
class idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::References {

    friend class impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>;
    using ParentT = impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>;

    ParentT* m_parent;

    explicit References(ParentT* t_parent) : m_parent(t_parent) {}
public:
    class iterator {
        friend class References;
        typename ParentT::MapType::iterator m_it;
    public:
        explicit iterator(typename ParentT::MapType::iterator&& t_it) : m_it(std::move(t_it)) {}
        explicit iterator(const typename ParentT::MapType::iterator& t_it) : m_it(t_it) {}
        bool operator!=(const iterator& t_rhs) const { return m_it != t_rhs.m_it; }
        bool operator==(const iterator& t_rhs) const { return m_it == t_rhs.m_it; }
        iterator& operator++() { ++m_it; return *this; }
        std::pair<Key, MatrixCoefficientReference> operator*() const {
            return { m_it->first, MatrixCoefficientReference(*m_it->second) };
        }
    };

    iterator begin() { return iterator(m_parent->m_map.begin()); }
    iterator end() { return iterator(m_parent->m_map.end()); }

    iterator find(const Key& t_key) { return iterator(m_parent->m_map.find(t_key)); }

    iterator emplace(const Key& t_key, MatrixCoefficientReference&& t_coefficient) {
        auto [inserted, success] = m_parent->m_map.emplace(t_key, std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)));
        if (!success) { throw Exception("Could not insert reference."); }
        return iterator(inserted);
    }

    void set(const Key& t_key, MatrixCoefficientReference&& t_coefficient) {
        if (std::abs(t_coefficient.value()) < Tolerance::Sparsity) {
            m_parent->m_map.erase(t_key);
            return;
        }
        emplace(t_key, std::move(t_coefficient));
    }

    iterator erase(const iterator& t_it) {
        auto result = m_parent->m_map.erase(t_it.m_it);
        return iterator(result);
    }
};

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
typename idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::References idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::refs() {
    return References(this);
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
class idol::impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::const_iterator {
    typename AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::MapType::const_iterator m_it;
public:
    explicit const_iterator(const typename AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::MapType::const_iterator& t_it) : m_it(t_it) {}
    bool operator!=(const const_iterator& t_rhs) const { return m_it != t_rhs.m_it; }
    bool operator==(const const_iterator& t_rhs) const { return m_it == t_rhs.m_it; }
    const_iterator operator++() { ++m_it; return *this; }
    IteratorOutputT operator*() const { return IteratorOutputT(m_it->first, m_it->second->value()); }
};

template<class Key,
        class IteratorOutputT = idol::Pair<const Key&, double>,
        class Hash = std::hash<Key>,
        class EqualTo = std::equal_to<Key>
>
class idol::AbstractExpr : public impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> {
public:
    AbstractExpr() = default;
    AbstractExpr(const AbstractExpr& t_src) = default;
    AbstractExpr(AbstractExpr&& t_src) noexcept = default;

    AbstractExpr& operator=(const AbstractExpr& t_src) = default;
    AbstractExpr& operator=(AbstractExpr&& t_src) noexcept = default;
};

namespace idol {

    template<class Key, class IteratorOutputT, class Hash, class EqualTo>
    std::ostream &operator<<(std::ostream &t_os, const AbstractExpr <Key, IteratorOutputT, Hash, EqualTo> &t_expr) {

        if (t_expr.empty()) {
            return t_os << "0";
        }

        auto it = t_expr.begin();
        const auto end = t_expr.end();

        t_os << *it;

        for (++it; it != end; ++it) {
            t_os << " + " << *it;
        }

        return t_os;
    }

}

#endif //IDOL_ABSTRACTEXPR_H
