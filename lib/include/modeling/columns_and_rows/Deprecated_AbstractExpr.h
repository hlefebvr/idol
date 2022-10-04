//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_DEPRECATED_ABSTRACTEXPR_H
#define OPTIMIZE_DEPRECATED_ABSTRACTEXPR_H

#include "../coefficients/MatrixCoefficient.h"
#include "../coefficients/Constant.h"
#include "modeling/numericals.h"
#include "errors/Exception.h"
#include <memory>
#include <stdexcept>
#include <functional>
#include <list>

template<class Key>
class Deprecated_AbstractExpr {
protected:
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient>>;
    MapType m_map;

    template<class Iterator, class Output> class base_iterator;

    std::pair<MatrixCoefficientReference, bool> insert_or_update(const Key& t_key, Constant t_coefficient);
public:
    Deprecated_AbstractExpr() = default;
    Deprecated_AbstractExpr(Constant t_factor, const Key& t_key);

    virtual ~Deprecated_AbstractExpr() = default;

    Deprecated_AbstractExpr(const Deprecated_AbstractExpr& t_src);
    Deprecated_AbstractExpr(Deprecated_AbstractExpr&& t_src) noexcept = default;

    Deprecated_AbstractExpr& operator=(const Deprecated_AbstractExpr& t_src);
    Deprecated_AbstractExpr& operator=(Deprecated_AbstractExpr&& t_src) noexcept = default;

    virtual Deprecated_AbstractExpr& operator*=(double t_factor);
    Deprecated_AbstractExpr& operator+=(const Deprecated_AbstractExpr& t_expr);

    void set(const Key& t_key, Constant t_coefficient);

    void set(const Key& t_key, MatrixCoefficientReference&& t_coefficient);

    [[nodiscard]] unsigned int size() const;

    [[nodiscard]] const Constant& get(const Key& t_key) const;

    struct Entry;

    using const_iterator = base_iterator<typename MapType::const_iterator, Entry>;

    [[nodiscard]] const_iterator begin() const { return const_iterator(m_map.begin()); }
    [[nodiscard]] const_iterator end() const { return const_iterator(m_map.end()); }

    void replace_if(const std::function<std::optional<Deprecated_AbstractExpr<Key>>(const Key&)>& t_function);
};

template<class Key>
Deprecated_AbstractExpr<Key>::Deprecated_AbstractExpr(const Deprecated_AbstractExpr &t_src) {
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
}

template<class Key>
Deprecated_AbstractExpr<Key> &Deprecated_AbstractExpr<Key>::operator=(const Deprecated_AbstractExpr &t_src) {
    if (this == &t_src) { return *this; }
    m_map.clear();
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
    return *this;
}

template<class Key>
std::pair<MatrixCoefficientReference, bool>
Deprecated_AbstractExpr<Key>::insert_or_update(const Key &t_key, Constant t_coefficient) {
    if (t_coefficient.is_zero()) {
        m_map.erase(t_key);
        return { MatrixCoefficientReference(), true };
    }

    auto it = m_map.find(t_key);
    if (it == m_map.end()) {
        auto result = m_map.emplace(t_key, std::make_unique<MatrixCoefficient>(std::move(t_coefficient)));
        return { MatrixCoefficientReference(*result.first->second), true };
    }

    it->second->set_value(std::move(t_coefficient));
    return { MatrixCoefficientReference(*it->second), false };
}

template<class Key>
void Deprecated_AbstractExpr<Key>::set(const Key &t_key, Constant t_coefficient) {
    insert_or_update(t_key, std::move(t_coefficient));
}

template<class Key>
const Constant &Deprecated_AbstractExpr<Key>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? Constant::Zero : it->second->value();
}

template<class Key>
unsigned int Deprecated_AbstractExpr<Key>::size() const {
    return m_map.size();
}

template<class Key>
void Deprecated_AbstractExpr<Key>::set(const Key &t_key, MatrixCoefficientReference &&t_coefficient) {

    if (t_coefficient.empty()) {
        m_map.erase(t_key);
        return;
    }

    auto [it, success] = m_map.template emplace(t_key, std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)));
    if (!success) {
        throw Exception("Trying to insert a matrix coefficient by reference on an existing coefficient.");
    }
}

template<class Key>
Deprecated_AbstractExpr<Key>::Deprecated_AbstractExpr(Constant t_factor, const Key &t_key) {
    set(t_key, std::move(t_factor));
}

template<class Key>
Deprecated_AbstractExpr<Key> &Deprecated_AbstractExpr<Key>::operator*=(double t_factor) {
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
Deprecated_AbstractExpr<Key> &Deprecated_AbstractExpr<Key>::operator+=(const Deprecated_AbstractExpr& t_expr) {
    for (const auto& [key, value] : t_expr) {
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
void Deprecated_AbstractExpr<Key>::replace_if(const std::function<std::optional<Deprecated_AbstractExpr<Key>>(const Key &)> &t_function) {
    std::list<Deprecated_AbstractExpr<Key>> to_add;

    auto it = m_map.begin();
    const auto end = m_map.end();
    while (it != end) {
        if (auto expr = t_function(it->first) ; expr.has_value()) {
            to_add.template emplace_back(std::move(std::move(expr).value()));
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
struct Deprecated_AbstractExpr<Key>::Entry {
    Key first;
    const Constant& second;
    explicit Entry(const std::pair<const Key, std::unique_ptr<AbstractMatrixCoefficient>>& t_pair) : first(t_pair.first), second(t_pair.second->value()) {}
    explicit Entry(const Key& t_key, const Constant& t_coeff) : first(t_key), second(t_coeff) {}
};

template<class Key>
template<class Iterator, class Output>
class Deprecated_AbstractExpr<Key>::base_iterator {
    Iterator m_it;
public:
    explicit base_iterator(Iterator&& t_it) : m_it(std::move(t_it)) {}
    bool operator!=(const base_iterator& t_rhs) { return m_it != t_rhs.m_it; }
    bool operator==(const base_iterator& t_rhs) { return m_it == t_rhs.m_it; }
    base_iterator& operator++() { ++m_it; return *this; }
    Output operator*() const { return Output(*m_it); }
};

template<class Key>
std::ostream &operator<<(std::ostream& t_os, const Deprecated_AbstractExpr<Key>& t_column_or_row) {

    const auto print_term = [&t_os](const Key& t_key, const Constant& t_coeff) {
        if (t_coeff.is_numerical()) {
            if (!equals(t_coeff.constant(), 1., ToleranceForSparsity)) {
                t_os << t_coeff << ' ';
            }
        } else if(equals(t_coeff.constant(), 0., ToleranceForSparsity) && t_coeff.size() == 1) {
            t_os << t_coeff << ' ';
        } else {
            t_os << '(' << t_coeff << ") ";
        }
        t_os << t_key;
    };

    auto it = t_column_or_row.begin();
    const auto end = t_column_or_row.end();

    if (it == end) {
        return t_os << '0';
    }

    print_term((*it).first, (*it).second);

    for (++it ; it != end ; ++it) {
        t_os << " + ";
        print_term((*it).first, (*it).second);
    }

    return t_os;
}

#endif //OPTIMIZE_DEPRECATED_ABSTRACTEXPR_H
