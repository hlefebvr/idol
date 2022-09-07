//
// Created by henri on 06/09/22.
//

#ifndef OPTIMIZE_COLUMNORROW_H
#define OPTIMIZE_COLUMNORROW_H

#include <iostream>
#include <memory>
#include "containers/Map.h"
#include "MatrixCoefficient.h"

template<class Key, enum Player PlayerT>
class ColumnOrRow {
    using MapType = Map<Key, std::unique_ptr<AbstractMatrixCoefficient<PlayerT>>>;
    MapType m_map;
    std::unique_ptr<AbstractMatrixCoefficient<PlayerT>> m_constant;

    template<class Iterator, class Entry> class base_iterator;
public:
    ColumnOrRow() = default;

    ColumnOrRow(const ColumnOrRow<Key, PlayerT>& t_src) = delete;
    ColumnOrRow(ColumnOrRow<Key, PlayerT>&& t_src) noexcept = default;

    ColumnOrRow& operator=(const ColumnOrRow<Key, PlayerT>& t_src) = delete;
    ColumnOrRow& operator=(ColumnOrRow<Key, PlayerT>&& t_src) noexcept = default;

    MatrixCoefficientReference<PlayerT> set(const Key& t_key, LinExpr<opp_player_v<PlayerT>> t_value);
    MatrixCoefficientReference<PlayerT> set(const Key& t_key, MatrixCoefficientReference<PlayerT> t_ref);

    MatrixCoefficientReference<PlayerT> set_constant(LinExpr<opp_player_v<PlayerT>> t_value);
    MatrixCoefficientReference<PlayerT> set_constant(MatrixCoefficientReference<PlayerT> t_ref);

    const LinExpr<opp_player_v<PlayerT>>& get(const Key& t_key) const;
    const LinExpr<opp_player_v<PlayerT>>& get_constant() const;

    struct entry;
    struct const_entry;

    using iterator = base_iterator<typename MapType::iterator, entry>;
    using const_iterator = base_iterator<typename MapType::const_iterator, const_entry>;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
};

template<class Key, enum Player PlayerT>
MatrixCoefficientReference<PlayerT>
ColumnOrRow<Key, PlayerT>::set(const Key &t_key, LinExpr<opp_player_v<PlayerT>> t_value) {

    auto it = m_map.find(t_key);

    if (it == m_map.end()) {
        auto coefficient = std::make_unique<MatrixCoefficient<PlayerT>>(std::move(t_value));
        MatrixCoefficientReference<PlayerT> reference(*coefficient);
        m_map.template emplace(t_key, std::move(coefficient));
        return reference;
    }

    it->second->set_value(std::move(t_value));

    return MatrixCoefficientReference<PlayerT>(*it->second);
}

template<class Key, enum Player PlayerT>
MatrixCoefficientReference<PlayerT> ColumnOrRow<Key, PlayerT>::set(const Key &t_key, MatrixCoefficientReference<PlayerT> t_ref) {

    auto coefficient = std::make_unique<MatrixCoefficientReference<PlayerT>>(std::move(t_ref));
    MatrixCoefficientReference<PlayerT> reference(t_ref);

    auto it = m_map.find(t_key);
    if (it == m_map.end()) {
        m_map.template emplace(t_key, std::move(coefficient));
    } else {
        it->second = std::move(coefficient);
    }

    return reference;

}

template<class Key, enum Player PlayerT>
const LinExpr<opp_player_v<PlayerT>> &ColumnOrRow<Key, PlayerT>::get(const Key &t_key) const {
    auto it = m_map.find(t_key);
    return it == m_map.end() ? LinExpr<opp_player_v<PlayerT>>::Zero : it->second->value();
}

template<class Key, enum Player PlayerT>
MatrixCoefficientReference<PlayerT> ColumnOrRow<Key, PlayerT>::set_constant(LinExpr<opp_player_v<PlayerT>> t_value) {
    if (!m_constant) {
        m_constant = std::make_unique<MatrixCoefficient<PlayerT>>(std::move(t_value));
    } else {
        m_constant->set_value(std::move(t_value));
    }
    return MatrixCoefficientReference<PlayerT>(*m_constant);
}

template<class Key, enum Player PlayerT>
MatrixCoefficientReference<PlayerT> ColumnOrRow<Key, PlayerT>::set_constant(MatrixCoefficientReference<PlayerT> t_ref) {
    m_constant = std::make_unique<MatrixCoefficientReference<PlayerT>>(std::move(t_ref));
    return MatrixCoefficientReference<PlayerT>(*m_constant);
}

template<class Key, enum Player PlayerT>
const LinExpr<opp_player_v<PlayerT>> &ColumnOrRow<Key, PlayerT>::get_constant() const {
    return m_constant->value();
}

template<class Key, enum Player PlayerT>
typename ColumnOrRow<Key, PlayerT>::iterator ColumnOrRow<Key, PlayerT>::begin() {
    return ColumnOrRow::iterator(m_map.begin());
}

template<class Key, enum Player PlayerT>
typename ColumnOrRow<Key, PlayerT>::iterator ColumnOrRow<Key, PlayerT>::end() {
    return ColumnOrRow::iterator(m_map.end());
}

template<class Key, enum Player PlayerT>
typename ColumnOrRow<Key, PlayerT>::const_iterator ColumnOrRow<Key, PlayerT>::begin() const {
    return ColumnOrRow::const_iterator(m_map.begin());
}

template<class Key, enum Player PlayerT>
typename ColumnOrRow<Key, PlayerT>::const_iterator ColumnOrRow<Key, PlayerT>::end() const {
    return ColumnOrRow::const_iterator(m_map.end());
}

template<class Key, enum Player PlayerT>
typename ColumnOrRow<Key, PlayerT>::const_iterator ColumnOrRow<Key, PlayerT>::cbegin() const {
    return ColumnOrRow::const_iterator(m_map.begin());
}

template<class Key, enum Player PlayerT>
typename ColumnOrRow<Key, PlayerT>::const_iterator ColumnOrRow<Key, PlayerT>::cend() const {
    return ColumnOrRow::const_iterator(m_map.end());
}

template<class Key, enum Player PlayerT>
struct ColumnOrRow<Key, PlayerT>::entry {
    const Key& m_key;
    LinExpr<opp_player_v<PlayerT>>& m_value;
    entry(const Key& t_key, LinExpr<opp_player_v<PlayerT>>& t_value);
};

template<class Key, enum Player PlayerT>
ColumnOrRow<Key, PlayerT>::entry::entry(const Key& t_key, LinExpr<opp_player_v<PlayerT>>& t_value)
        : m_key(t_key), m_value(t_value) {

}

template<class Key, enum Player PlayerT>
struct ColumnOrRow<Key, PlayerT>::const_entry {
    const Key& m_key;
    const LinExpr<opp_player_v<PlayerT>>& m_value;
    const_entry(const Key& t_key, const LinExpr<opp_player_v<PlayerT>>& t_value);
};

template<class Key, enum Player PlayerT>
ColumnOrRow<Key, PlayerT>::const_entry::const_entry(const Key& t_key, const LinExpr<opp_player_v<PlayerT>>& t_value)
        : m_key(t_key), m_value(t_value) {

}

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
class ColumnOrRow<Key, PlayerT>::base_iterator {
    Iterator m_it;
    friend class ColumnOrRow<Key, PlayerT>;
    explicit base_iterator(Iterator&& t_it);
public:
    base_iterator(const base_iterator&) = default;
    base_iterator(base_iterator&&) = default;

    base_iterator& operator=(const base_iterator&) = default;
    base_iterator& operator=(base_iterator&&) = default;

    bool operator!=(const base_iterator& t_rhs) const;
    bool operator==(const base_iterator& t_rhs) const;
    base_iterator& operator++();
    base_iterator operator++(int) &;
    Entry operator*() const;
};

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
ColumnOrRow<Key, PlayerT>::base_iterator<Iterator, Entry>::base_iterator(Iterator &&t_it) : m_it(std::move(t_it)) {

}

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
bool
ColumnOrRow<Key, PlayerT>::base_iterator<Iterator, Entry>::operator!=(const base_iterator &t_rhs) const {
    return m_it != t_rhs.m_it;
}

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
bool
ColumnOrRow<Key, PlayerT>::base_iterator<Iterator, Entry>::operator==(const base_iterator &t_rhs) const {
    return m_it == t_rhs.m_it;
}

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
typename ColumnOrRow<Key, PlayerT>::template base_iterator<Iterator, Entry>&
ColumnOrRow<Key, PlayerT>::base_iterator<Iterator, Entry>::operator++() {
    ++m_it;
    return *this;
}

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
typename ColumnOrRow<Key, PlayerT>::template base_iterator<Iterator, Entry>
ColumnOrRow<Key, PlayerT>::base_iterator<Iterator, Entry>::operator++(int) & {
    auto result = *this;
    ++m_it;
    return result;
}

template<class Key, enum Player PlayerT>
template<class Iterator, class Entry>
Entry
ColumnOrRow<Key, PlayerT>::base_iterator<Iterator, Entry>::operator*() const {
    return Entry(m_it->first, m_it->second->value());
}


#endif //OPTIMIZE_COLUMNORROW_H
