//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_DEPRECATED_COLUMNORROWREFERENCE_H
#define OPTIMIZE_DEPRECATED_COLUMNORROWREFERENCE_H

#include <vector>
#include "modeling/columns_and_rows/Deprecated_ColumnOrRow.h"

namespace impl {
    template<class Key> class Deprecated_ColumnOrRowReference;
}

template<class Key>
class impl::Deprecated_ColumnOrRowReference {
    Deprecated_ColumnOrRow<Key>& m_column;
protected:
    explicit Deprecated_ColumnOrRowReference(Deprecated_ColumnOrRow<Key>& t_column) : m_column(t_column) {}
public:
    struct Entry;
    using iterator = typename Deprecated_ColumnOrRow<Key>::template base_iterator<typename Deprecated_ColumnOrRow<Key>::MapType::iterator, Entry>;

    iterator begin() { return iterator(m_column.m_map.begin()); }
    iterator end() { return iterator(m_column.m_map.end()); }

    std::pair<MatrixCoefficientReference, bool> set(const Key& t_key, Constant t_coefficient);

    MatrixCoefficientReference constant();
};

template<class Key>
MatrixCoefficientReference impl::Deprecated_ColumnOrRowReference<Key>::constant() {
    return MatrixCoefficientReference(*m_column.m_constant);
}

template<class Key>
std::pair<MatrixCoefficientReference, bool>
impl::Deprecated_ColumnOrRowReference<Key>::set(const Key &t_key, Constant t_coefficient) {
    return m_column.insert_or_update(t_key, std::move(t_coefficient));
}

template<class Key>
struct impl::Deprecated_ColumnOrRowReference<Key>::Entry {
    Key first;
    MatrixCoefficientReference second;
    explicit Entry(const std::pair<const Key, std::unique_ptr<AbstractMatrixCoefficient>>& t_pair) : first(t_pair.first), second(*t_pair.second) {}
};


template<class Key>
class ColumnOrRowReference : public impl::Deprecated_ColumnOrRowReference<Key> {
    friend class Model;

    explicit ColumnOrRowReference(Deprecated_ColumnOrRow<Key>& t_column) : impl::Deprecated_ColumnOrRowReference<Key>(t_column) {}
};

#endif //OPTIMIZE_DEPRECATED_COLUMNORROWREFERENCE_H
