//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_ABSTRACTEXPR_H
#define OPTIMIZE_ABSTRACTEXPR_H

#include "containers/Map.h"
#include "../coefficients/AbstractMatrixCoefficient.h"
#include "../coefficients/MatrixCoefficient.h"
#include "../coefficients/Coefficient.h"
#include <memory>
#include <stdexcept>

template<class Key>
class AbstractExpr {
protected:
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient>>;
    MapType m_map;

    template<class Iterator, class Output> class base_iterator;
public:
    AbstractExpr() = default;

    virtual ~AbstractExpr() = default;

    AbstractExpr(const AbstractExpr& t_src);
    AbstractExpr(AbstractExpr&& t_src) noexcept = default;

    AbstractExpr& operator=(const AbstractExpr& t_src);
    AbstractExpr& operator=(AbstractExpr&& t_src) noexcept = default;

    void set(const Key& t_key, Coefficient t_coefficient);

    void set(const Key& t_key, MatrixCoefficientReference&& t_coefficient);

    [[nodiscard]] unsigned int size() const;

    [[nodiscard]] const Coefficient& get(const Key& t_key) const;

    struct Entry;

    using const_iterator = base_iterator<typename MapType::const_iterator, Entry>;

    [[nodiscard]] const_iterator begin() const { return const_iterator(m_map.begin()); }
    [[nodiscard]] const_iterator end() const { return const_iterator(m_map.end()); }
};

template<class Key>
AbstractExpr<Key>::AbstractExpr(const AbstractExpr &t_src) {
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
}

template<class Key>
AbstractExpr<Key> &AbstractExpr<Key>::operator=(const AbstractExpr &t_src) {
    if (this == &t_src) { return *this; }
    m_map.clear();
    for (const auto& [key, ptr_to_value] : t_src.m_map) {
        m_map.template emplace(key, std::make_unique<MatrixCoefficient>(ptr_to_value->value()));
    }
    return *this;
}

template<class Key>
void AbstractExpr<Key>::set(const Key &t_key, Coefficient t_coefficient) {

    if (t_coefficient.empty()) {
        m_map.erase(t_key);
        return;
    }

    auto it = m_map.find(t_key);
    if (it == m_map.end()) {
        m_map.emplace(t_key, std::make_unique<MatrixCoefficient>(std::move(t_coefficient)));
    } else {
        it->second->set_value(std::move(t_coefficient));
    }

}

template<class Key>
const Coefficient &AbstractExpr<Key>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? Coefficient::Zero : it->second->value();
}

template<class Key>
unsigned int AbstractExpr<Key>::size() const {
    return m_map.size();
}

template<class Key>
void AbstractExpr<Key>::set(const Key &t_key, MatrixCoefficientReference &&t_coefficient) {
    auto [it, success] = m_map.template emplace(t_key, std::make_unique<MatrixCoefficientReference>(std::move(t_coefficient)));
    if (!success) {
        throw std::runtime_error("Trying to insert a matrix coefficient by reference on an existing coefficient.");
    }
}

template<class Key>
struct AbstractExpr<Key>::Entry {
    Key first;
    const Coefficient& second;
    explicit Entry(const std::pair<const Key, std::unique_ptr<AbstractMatrixCoefficient>>& t_pair) : first(t_pair.first), second(t_pair.second->value()) {}
};

template<class Key>
template<class Iterator, class Output>
class AbstractExpr<Key>::base_iterator {
    Iterator m_it;
public:
    explicit base_iterator(Iterator&& t_it) : m_it(std::move(t_it)) {}
    bool operator!=(const base_iterator& t_rhs) { return m_it != t_rhs.m_it; }
    bool operator==(const base_iterator& t_rhs) { return m_it == t_rhs.m_it; }
    base_iterator& operator++() { ++m_it; return *this; }
    Output operator*() const { return Output(*m_it); }
};

template<class Key>
std::ostream &operator<<(std::ostream& t_os, const AbstractExpr<Key>& t_column_or_row) {
    t_os << t_column_or_row.constant();
    for (const auto& [key, value] : t_column_or_row) {
        t_os << " + ";
        if (value.is_numerical()) {
            t_os << value;
        } else {
            t_os << '(' << value << ')';
        }
        t_os << " * " << key;
    }
    return t_os;
}

#endif //OPTIMIZE_ABSTRACTEXPR_H
