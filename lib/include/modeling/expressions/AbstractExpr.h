//
// Created by henri on 21/11/22.
//

#ifndef IDOL_ABSTRACTEXPR_H
#define IDOL_ABSTRACTEXPR_H

#include "../../containers/Map.h"
#include "../../containers/IteratorForward.h"
#include "../matrix/AbstractMatrixCoefficient.h"
#include "Constant.h"
#include "./operations/operators_Constant.h"
#include "../matrix/MatrixCoefficient.h"
#include "../variables/Var.h"
#include "../../errors/Exception.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

namespace impl {
    template<class, class, class, class> class AbstractExpr;
}

template<class Key,
        class IteratorOutputT,
        class Hash,
        class EqualTo>
class impl::AbstractExpr {
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient>, Hash, EqualTo>;
    MapType m_map;
protected:
    void set(const Key& t_key, Constant&& t_coefficient);
    const Constant& get(const Key& t_key) const;
    void remove(const Key& t_key);

    class References;
    References refs();
public:
    AbstractExpr() = default;

    /**
     * Copy constructor. Every terms are copied by value.
     * @param t_src The source object to copy.
     */
    AbstractExpr(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_src);

    /**
     * Move constructor. Every terms are moved.
     * Meaning that MatrixCoefficientReference-s remain MatrixCoefficientReference-s, if any.
     * @param t_src
     */
    AbstractExpr(AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>&& t_src) noexcept = default;

    /**
     * Assignment operator. Every term is copied by value.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator=(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_src);

    /**
     * Assignment move operator. Every term is moved.
     * Meaning that MatrixCoefficientReference-s remain MatrixCoefficientReference-s, if any.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator=(AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>&& t_src) noexcept = default;

    /**
     * Adds up every term of t_rhs to the Expr.
     * Zero entries are removed from the resulting Expr.
     * @param t_rhs The object whose terms are to be added up.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator+=(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_rhs);

    /**
     * Subtracts every term of t_rhs to the Expr.
     * Zero entries are removed from the resulting Expr.
     * @param t_rhs The object whose terms are to be subtracted.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator-=(const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_rhs);

    /**
     * Multiplies every term of the Expr by t_factor. If t_factor equals zero with tolerance ToleranceForSparsity, then the Expr is emptied.
     * @param t_factor The multiplying factor.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& operator*=(double t_factor);

    /**
     * Returns the size of the Expr, i.e., the number of non-zero entries which are currently stored.
     */
    [[nodiscard]] unsigned int size() const { return m_map.size(); }

    /**
     * Returns True if the size is zero, False otherwise.
     */
    [[nodiscard]] bool empty() const { return m_map.empty(); }

    class const_iterator;

    const_iterator begin() const { return const_iterator(m_map.begin()); }
    const_iterator end() const { return const_iterator(m_map.end()); }

};

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::AbstractExpr(const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_src) {

    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator=(const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_src) {

    if (this == &t_src) { return *this; }
    m_map.clear();
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
    return *this;

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator+=(const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_rhs) {

    // TODO remove if sum is zero

    for (const auto& [key, ptr_to_value] : t_rhs.m_map) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
        } else {
            it->second->value() += ptr_to_value->value();
        }
    }
    return *this;

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator-=(
        const impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &t_rhs) {

    // TODO: remove if diff is zero

    for (const auto& [key, ptr_to_value] : t_rhs.m_map) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_map.template emplace(key, std::make_unique<MatrixCoefficient>(-ptr_to_value->value()));
        } else {
            it->second->value() -= ptr_to_value->value();
        }
    }
    return *this;

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> &
impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::operator*=(double t_factor) {

    if (equals(t_factor, 0., ToleranceForSparsity)) {
        m_map.clear();
        return *this;
    }

    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value() *= t_factor;
    }

    return *this;
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
void impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::set(const Key &t_key, Constant &&t_coefficient) {

    if (t_coefficient.is_zero()) {
        m_map.erase(t_key);
        return;
    }

    auto it = m_map.find(t_key);
    if (it == m_map.end()) {
        m_map.emplace(t_key, std::make_unique<MatrixCoefficient>(std::move(t_coefficient)));
        return;
    }

    it->second->set_value(std::move(t_coefficient));

}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
const Constant &impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? Constant::Zero : it->second->value();
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
void impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::remove(const Key &t_key) {
    m_map.erase(t_key);
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
class impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::References {

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
        if (t_coefficient.value().is_zero()) {
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
typename impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::References impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::refs() {
    return References(this);
}

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
class impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::const_iterator {
    typename AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::MapType::const_iterator m_it;
public:
    explicit const_iterator(const typename AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>::MapType::const_iterator& t_it) : m_it(t_it) {}
    bool operator!=(const const_iterator& t_rhs) const { return m_it != t_rhs.m_it; }
    bool operator==(const const_iterator& t_rhs) const { return m_it == t_rhs.m_it; }
    const_iterator operator++() { ++m_it; return *this; }
    IteratorOutputT operator*() const { return { m_it->first, m_it->second->value() }; }
};

template<class Key,
        class IteratorOutputT = std::pair<const Key&, const Constant&>,
        class Hash = std::hash<Key>,
        class EqualTo = std::equal_to<Key>
>
class AbstractExpr : public impl::AbstractExpr<Key, IteratorOutputT, Hash, EqualTo> {
public:
    AbstractExpr() = default;
    AbstractExpr(const AbstractExpr& t_src) = default;
    AbstractExpr(AbstractExpr&& t_src) noexcept = default;

    AbstractExpr& operator=(const AbstractExpr& t_src) = default;
    AbstractExpr& operator=(AbstractExpr&& t_src) noexcept = default;
};

template<class Key, class IteratorOutputT, class Hash, class EqualTo>
std::ostream& operator<<(std::ostream& t_os, const AbstractExpr<Key, IteratorOutputT, Hash, EqualTo>& t_expr) {

    if (t_expr.empty()) {
        return t_os << "0";
    }

    auto it = t_expr.begin();
    const auto end = t_expr.end();

    t_os << *it;

    for (++it ; it != end ; ++it) {
        t_os << " + " << *it;
    }

    return t_os;
}


#endif //IDOL_ABSTRACTEXPR_H
