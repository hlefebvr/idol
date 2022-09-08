//
// Created by henri on 08/09/22.
//

#ifndef OPTIMIZE_COLUMNORROWREFERENCE_H
#define OPTIMIZE_COLUMNORROWREFERENCE_H

#include <vector>
#include "modeling/columns_and_rows/ColumnOrRow.h"

namespace impl {
    template<class Key> class ColumnOrRowReference;
}

template<class Key>
class impl::ColumnOrRowReference {
    ColumnOrRow<Key>& m_column;
protected:
    explicit ColumnOrRowReference(ColumnOrRow<Key>& t_column) : m_column(t_column) {}
public:
    struct Entry;
    using iterator = typename ColumnOrRow<Key>::template base_iterator<typename ColumnOrRow<Key>::MapType::iterator, Entry>;

    iterator begin() { return iterator(m_column.m_map.begin()); }
    iterator end() { return iterator(m_column.m_map.end()); }

    MatrixCoefficientReference constant();
};

template<class Key>
MatrixCoefficientReference impl::ColumnOrRowReference<Key>::constant() {
    return MatrixCoefficientReference(*m_column.m_constant);
}

template<class Key>
struct impl::ColumnOrRowReference<Key>::Entry {
    Key first;
    MatrixCoefficientReference second;
    explicit Entry(const std::pair<const Key, std::unique_ptr<AbstractMatrixCoefficient>>& t_pair) : first(t_pair.first), second(*t_pair.second) {}
};


template<class Key>
class ColumnOrRowReference : public impl::ColumnOrRowReference<Key> {
    friend class Model;

    explicit ColumnOrRowReference(ColumnOrRow<Key>& t_column) : impl::ColumnOrRowReference<Key>(t_column) {}
};

#endif //OPTIMIZE_COLUMNORROWREFERENCE_H
