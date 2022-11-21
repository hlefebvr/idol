//
// Created by henri on 21/11/22.
//

#ifndef IDOL_ABSTRACTEXPR_H
#define IDOL_ABSTRACTEXPR_H

#include "../../containers/Map.h"
#include "../../containers/IteratorForward.h"
#include "../matrix/AbstractMatrixCoefficient.h"
#include "Constant.h"
#include "operators_Constant.h"
#include "../matrix/MatrixCoefficient.h"
#include "../variables/Var.h"
#include "../../errors/Exception.h"
#include <memory>
#include <utility>
#include <functional>
#include <optional>
#include <list>

template<class Key, class IteratorOutputT = std::pair<const Key&, const Constant&>>
class AbstractExpr {
protected:
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient>>;
    MapType m_map;

    bool set(const Key& t_key, Constant&& t_coefficient);
    const Constant& get(const Key& t_key) const;
    void remove(const Key& t_key);
public:
    AbstractExpr() = default;

    /**
     * Copy constructor. Every terms are copied by value.
     * @param t_src The source object to copy.
     */
    AbstractExpr(const AbstractExpr<Key, IteratorOutputT>& t_src);

    /**
     * Move constructor. Every terms are moved.
     * Meaning that MatrixCoefficientReference-s remain MatrixCoefficientReference-s, if any.
     * @param t_src
     */
    AbstractExpr(AbstractExpr<Key, IteratorOutputT>&& t_src) noexcept = default;

    /**
     * Assignment operator. Every term is copied by value.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT>& operator=(const AbstractExpr<Key, IteratorOutputT>& t_src);

    /**
     * Assignment move operator. Every term is moved.
     * Meaning that MatrixCoefficientReference-s remain MatrixCoefficientReference-s, if any.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT>& operator=(AbstractExpr<Key, IteratorOutputT>&& t_src) noexcept = default;

    /**
     * Adds up every term of t_rhs to the Expr.
     * Zero entries are removed from the resulting Expr.
     * @param t_rhs The object whose terms are to be added up.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT>& operator+=(const AbstractExpr<Key, IteratorOutputT>& t_rhs);

    /**
     * Subtracts every term of t_rhs to the Expr.
     * Zero entries are removed from the resulting Expr.
     * @param t_rhs The object whose terms are to be subtracted.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT>& operator-=(const AbstractExpr<Key, IteratorOutputT>& t_rhs);

    /**
     * Multiplies every term of the Expr by t_factor. If t_factor equals zero with tolerance ToleranceForSparsity, then the Expr is emptied.
     * @param t_factor The multiplying factor.
     * @return The object itself.
     */
    AbstractExpr<Key, IteratorOutputT>& operator*=(double t_factor);

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

    /**
     * Updates the coefficient associated to t_key. The inserted coefficient is a reference to an existing coefficient in the matrix.
     * If t_key already has a coefficient, the method throws an exception.
     *
     * Note that this method is used to create the internal matrix of a model by linking constraints and columns.
     * A MatrixCoefficientReference should only be created by the Model iteself, when calling Model::add_ctr or Model::add_var.
     * @param t_key
     * @param t_coefficient
     * @return True (to agree with the description of template<class Key> Expr<Key>::set(const Key& t_key, Constant t_coefficient)).
     */
    bool set(const Key& t_key, MatrixCoefficientReference&& t_coefficient);
};

template<class Key, class IteratorOutputT>
bool AbstractExpr<Key, IteratorOutputT>::set(const Key &t_key, Constant &&t_coefficient) {

    if (t_coefficient.is_zero()) {
        m_map.erase(t_key);
        return true;
    }

    auto it = m_map.find(t_key);
    if (it == m_map.end()) {
        m_map.emplace(t_key, std::make_unique<MatrixCoefficient>(std::move(t_coefficient)));
        return true;
    }

    it->second->set_value(std::move(t_coefficient));
    return false;
}

template<class Key, class IteratorOutputT>
AbstractExpr<Key, IteratorOutputT>::AbstractExpr(const AbstractExpr<Key, IteratorOutputT> &t_src) {
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
}

template<class Key, class IteratorOutputT>
AbstractExpr<Key, IteratorOutputT> &AbstractExpr<Key, IteratorOutputT>::operator=(const AbstractExpr<Key, IteratorOutputT> &t_src) {
    if (this == &t_src) { return *this; }
    m_map.clear();
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
    return *this;
}

template<class Key, class IteratorOutputT>
AbstractExpr<Key, IteratorOutputT> &AbstractExpr<Key, IteratorOutputT>::operator+=(const AbstractExpr<Key, IteratorOutputT> &t_rhs) {
    for (const auto& [key, value] : t_rhs) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_map.template emplace(key, std::make_unique<MatrixCoefficient>(value));
        } else {
            it->second->value() += value;
        }
    }
    return *this;
}

template<class Key, class IteratorOutputT>
AbstractExpr<Key, IteratorOutputT> &AbstractExpr<Key, IteratorOutputT>::operator-=(const AbstractExpr<Key, IteratorOutputT> &t_rhs) {
    for (const auto& [key, value] : t_rhs) {
        auto it = m_map.find(key);
        if (it == m_map.end()) {
            m_map.template emplace(key, std::make_unique<MatrixCoefficient>(-value));
        } else {
            it->second->value() -= value;
        }
    }
    return *this;
}

template<class Key, class IteratorOutputT>
AbstractExpr<Key, IteratorOutputT> &AbstractExpr<Key, IteratorOutputT>::operator*=(double t_factor) {
    if (equals(t_factor, 0., ToleranceForSparsity)) {
        m_map.clear();
        return *this;
    }
    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value() *= t_factor;
    }
    return *this;
}

template<class Key, class IteratorOutputT>
void AbstractExpr<Key, IteratorOutputT>::remove(const Key &t_key) {
    m_map.erase(t_key);
}

template<class Key, class IteratorOutputT>
const Constant &AbstractExpr<Key, IteratorOutputT>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? Constant::Zero : it->second->value();
}

template<class Key, class IteratorOutputT>
bool AbstractExpr<Key, IteratorOutputT>::set(const Key &t_key, MatrixCoefficientReference &&t_coefficient) {
    if (t_coefficient.empty()) {
        this->m_map.erase(t_key);
        return true;
    }

    auto [it, success] = m_map.template emplace(t_key, std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)));
    if (!success) {
        throw Exception("Trying to insert a matrix coefficient by reference on an existing coefficient.");
    }
    return true;
}


template<class Key, class IteratorOutputT>
class AbstractExpr<Key, IteratorOutputT>::const_iterator {
    typename AbstractExpr<Key, IteratorOutputT>::MapType::const_iterator m_it;
public:
    explicit const_iterator(const typename AbstractExpr<Key, IteratorOutputT>::MapType::const_iterator& t_it) : m_it(t_it) {}
    bool operator!=(const const_iterator& t_rhs) const { return m_it != t_rhs.m_it; }
    bool operator==(const const_iterator& t_rhs) const { return m_it == t_rhs.m_it; }
    const_iterator operator++() { ++m_it; return *this; }
    IteratorOutputT operator*() const {
        return { m_it->first, m_it->second->value() };
    }
};


template<class Key, class IteratorOutputT>
std::ostream& operator<<(std::ostream& t_os, const AbstractExpr<Key, IteratorOutputT>& t_expr) {

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
