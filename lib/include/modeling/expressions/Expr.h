//
// Created by henri on 03/10/22.
//

#ifndef OPTIMIZE_EXPR_H
#define OPTIMIZE_EXPR_H

#include "../../containers/Map.h"
#include "../../containers/IteratorForward.h"
#include "../matrix/AbstractMatrixCoefficient.h"
#include "../matrix/Constant.h"
#include "../matrix/MatrixCoefficient.h"
#include "../variables/Variable.h"
#include "../../errors/Exception.h"
#include <memory>
#include <utility>
#include <functional>
#include <list>

/**
 * Numerical expression object.
 *
 * Typically represents the scalar product between a vector of Var and a vector of Constant.
 * This class is also used to create Row and Column.
 *
 * **Example 1**:
 * ```
 * Var x = ...;
 * Var y = ...;
 * Expr expr = 2 * x + 3 * y;
 * ```
 *
 * **Example 2**:
 * ```
 * Var x = ...;
 * Var y = ...;
 * Param xi = ...;
 * Expr expr = (2 + xi) * x + y:
 * ```
 *
 * **Example 3**:
 * ```
 * Ctr c1 = ...;
 * Ctr c2 = ...;
 * Expr<Ctr> expr = 2 * c1 + 3 * c2;
 * ```
 *
 * @tparam Key The type of elements for the first vector (the second vector type is always Constant).
 */
template<class Key = Var>
class Expr {
    friend class Matrix;
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient>>;
    MapType m_map;
public:
    Expr() = default;
    Expr(const Key& t_key); // NOLINT(google-explicit-constructor)
    Expr(Constant t_factor, const Key& t_key);

    /**
     * Copy constructor. Every terms are copied by value.
     * @param t_src The source object to copy.
     */
    Expr(const Expr<Key>& t_src);

    /**
     * Move constructor. Every terms are moved.
     * Meaning that MatrixCoefficientReference-s remain MatrixCoefficientReference-s, if any.
     * @param t_src
     */
    Expr(Expr<Key>&& t_src) noexcept = default;

    /**
     * Assignment operator. Every term is copied by value.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    Expr<Key>& operator=(const Expr<Key>& t_src);

    /**
     * Assignment move operator. Every term is moved.
     * Meaning that MatrixCoefficientReference-s remain MatrixCoefficientReference-s, if any.
     * @param t_src The source object to assign.
     * @return The object itself.
     */
    Expr<Key>& operator=(Expr<Key>&& t_src) noexcept = default;

    /**
     * Adds up every term of t_rhs to the Expr.
     * Zero entries are removed from the resulting Expr.
     * @param t_rhs The object whose terms are to be added up.
     * @return The object itself.
     */
    Expr<Key>& operator+=(const Expr<Key>& t_rhs);

    /**
     * Subtracts every term of t_rhs to the Expr.
     * Zero entries are removed from the resulting Expr.
     * @param t_rhs The object whose terms are to be subtracted.
     * @return The object itself.
     */
    Expr<Key>& operator-=(const Expr<Key>& t_rhs);

    /**
     * Multiplies every term of the Expr by t_factor. If t_factor equals zero with tolerance ToleranceForSparsity, then the Expr is emptied.
     * @param t_factor The multiplying factor.
     * @return The object itself.
     */
    Expr<Key>& operator*=(double t_factor);

    /**
     * Updates the coefficient associated to t_key.
     * @param t_key The key for which the coefficient should be modified.
     * @param t_coefficient The new value for the coefficient.
     * @return True if the stored size has been changed (i.e., if an entry has been removed or added) ;
     * False otherwise (i.e., if an entry has been modified).
     */
    bool set(const Key& t_key, Constant t_coefficient);

    /**
     * Updates the coefficient associated to t_key. The inserted coefficient is a reference to an existing coefficient in the matrix.
     * If t_key already has a coefficient, the method throws an exception.
     *
     * Note that this method is used to create the internal matrix of a model by linking constraints and columns.
     * A MatrixCoefficientReference should only be created by the Model iteself, when calling Model::add_constraint or Model::add_variable.
     * @param t_key
     * @param t_coefficient
     * @return True (to agree with the description of template<class Key> Expr<Key>::set(const Key& t_key, Constant t_coefficient)).
     */
    bool set(const Key& t_key, MatrixCoefficientReference&& t_coefficient);

    /**
     * Returns a const reference to the stored value associated to t_key. If no entry is found, a const-reference to Constant::Zero
     * is returned.
     * @param t_key The key for which a value is queried.
     */
    const Constant& get(const Key& t_key) const;

    /**
     * Returns the size of the Expr, i.e., the number of non-zero entries which are currently stored.
     */
    [[nodiscard]] unsigned int size() const { return m_map.size(); }

    /**
     * Returns True if the size is zero, False otherwise.
     */
    [[nodiscard]] bool empty() const { return m_map.empty(); }

    virtual ~Expr() = default;

    class const_iterator;

    const_iterator begin() const { return const_iterator(m_map.begin()); }
    const_iterator end() const { return const_iterator(m_map.end()); }

    /**
     * Replaces every a Key with the returned expression (if any) by t_function.
     *
     * Can be used to substitute, for instance, a Var by an Expr (e.g., \f$ x = \sum_e \alpha_e\textbf x^e \f$).
     * @param t_function A function which returns an optional expression to replace its argument.
     */
    void replace_if(const std::function<std::optional<Expr<Key>>(const Key&)>& t_function);

    /**
     * Removes the entry associated to the given key.
     * @param t_key The key for which the entry should be removed.
     */
    void remove(const Key& t_key);
};

template<class Key>
Expr<Key>::Expr(const Key &t_key) {
    set(t_key, 1.);
}

template<class Key>
Expr<Key>::Expr(Constant t_factor, const Key &t_key) {
    set(t_key, std::move(t_factor));
}

template<class Key>
Expr<Key>::Expr(const Expr<Key> &t_src) {
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
}

template<class Key>
Expr<Key> &Expr<Key>::operator=(const Expr<Key> &t_src) {
    if (this == &t_src) { return *this; }
    m_map.clear();
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
    return *this;
}

template<class Key>
Expr<Key> &Expr<Key>::operator+=(const Expr<Key> &t_rhs) {
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

template<class Key>
Expr<Key> &Expr<Key>::operator-=(const Expr<Key> &t_rhs) {
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

template<class Key>
Expr<Key> &Expr<Key>::operator*=(double t_factor) {
    if (equals(t_factor, 0., ToleranceForSparsity)) {
        m_map.clear();
        return *this;
    }
    for (const auto& [key, ptr_to_value] : m_map) {
        ptr_to_value->value() *= t_factor;
    }
    return *this;
}

template<class Key>
bool Expr<Key>::set(const Key &t_key, Constant t_coefficient) {

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

template<class Key>
bool Expr<Key>::set(const Key &t_key, MatrixCoefficientReference &&t_coefficient) {
    if (t_coefficient.empty()) {
        m_map.erase(t_key);
        return true;
    }

    auto [it, success] = m_map.template emplace(t_key, std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)));
    if (!success) {
        throw Exception("Trying to insert a matrix coefficient by reference on an existing coefficient.");
    }
    return true;
}

template<class Key>
const Constant &Expr<Key>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? Constant::Zero : it->second->value();
}

template<class Key>
void Expr<Key>::replace_if(const std::function<std::optional<Expr<Key>>(const Key &)> &t_function) {
    std::list<Expr<Key>> to_add;

    auto it = m_map.begin();
    const auto end = m_map.end();
    while (it != end) {
        if (auto result = t_function(it->first) ; result.has_value()) {
            auto expr = it->second->value().numerical() * result.value();
            to_add.template emplace_back(std::move(expr));
            it = m_map.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& expr : to_add) {
        *this += expr;
    }
}

template<class Key>
void Expr<Key>::remove(const Key &t_key) {
    m_map.erase(t_key);
}

template<class Key>
class Expr<Key>::const_iterator {
    typename Expr<Key>::MapType::const_iterator m_it;
public:
    explicit const_iterator(const typename Expr<Key>::MapType::const_iterator& t_it) : m_it(t_it) {}
    bool operator!=(const const_iterator& t_rhs) const { return m_it != t_rhs.m_it; }
    bool operator==(const const_iterator& t_rhs) const { return m_it == t_rhs.m_it; }
    const_iterator operator++() { ++m_it; return *this; }
    std::pair<const Key&, const Constant&> operator*() const {
        return { m_it->first, m_it->second->value() };
    }
};

static Expr<Var> operator+(const Var& t_a, const Var& t_b) {
    Expr<Var> result(t_a);
    result += t_b;
    return result;
}

static Expr<Var> operator+(Expr<Var> t_a, const Var& t_b) {
    Expr<Var> result(std::move(t_a));
    result += t_b;
    return result;
}

static Expr<Var> operator+(const Var& t_b, Expr<Var> t_a) {
    return std::move(t_a) + t_b;
}

template<class Key>
Expr<Key> operator*(double t_factor, const Key& t_var) {
    return { t_factor, t_var };
}

template<class Key>
Expr<Key> operator*(Constant t_factor, const Key& t_var) {
    return { std::move(t_factor), t_var };
}

template<class Key>
Expr<Key> operator*(double t_factor, Expr<Key> t_expr) {
    Expr<Key> result(std::move(t_expr));
    result *= t_factor;
    return result;
}

template<class Key>
Expr<Key> operator+(Expr<Key> t_a, const Expr<Key>& t_b) {
    Expr<Key> result(std::move(t_a));
    result += t_b;
    return result;
}

template<class Key>
Expr<Key> operator-(Expr<Key> t_a, const Expr<Key>& t_b) {
    Expr<Key> result(std::move(t_a));
    result -= t_b;
    return result;
}

template<class Key>
std::ostream& operator<<(std::ostream& t_os, const Expr<Key>& t_expr) {
    if (t_expr.empty()) {
        return t_os << 0.;
    }

    auto it = t_expr.begin();
    const auto end = t_expr.end();

    t_os << '{' << (*it).second << '}' << " " << (*it).first;

    for (++it ; it != end ; ++it) {
        t_os << " + " << '{' << (*it).second << '}' << " " << (*it).first;
    }

    return t_os;
}

#endif //OPTIMIZE_EXPR_H
