//
// Created by henri on 24.10.24.
//

#ifndef IDOL_SPARSEVECTOR_H
#define IDOL_SPARSEVECTOR_H

#include <vector>
#include <algorithm>
#include <optional>
#include "idol/containers/sort.h"

namespace idol {
    template<class, class, class>
    class SparseVector;
}

template<class IndexT, class ValueT, class IndexExtractorT = std::conditional_t<std::is_arithmetic_v<IndexT>, idol::identity<IndexT>, idol::get_id<IndexT>>>
class idol::SparseVector {
    static_assert(std::is_default_constructible_v<ValueT>);

    enum class SortingCriteria {
        Index,
        Value,
        None
    };

    std::vector<IndexT> m_indices;
    std::vector<ValueT> m_values;
    SortingCriteria m_sorting_criteria;
    bool m_is_reduced; // true if the vector does not contain duplicate indices

    SparseVector binary_operation_on_sorted_vectors(const SparseVector& t_vec1, const SparseVector& t_vec2, const std::function<ValueT(const ValueT&, const ValueT&)>& t_operation);
public:
    SparseVector();

    SparseVector(std::vector<IndexT> t_indices,
                 std::vector<ValueT> t_values,
                 SortingCriteria t_sorting_criteria = SortingCriteria::None,
                 bool t_is_reduced = false);

    SparseVector(const SparseVector&) = default;
    SparseVector(SparseVector&&) = default;

    SparseVector& operator=(const SparseVector&) = default;
    SparseVector& operator=(SparseVector&&) noexcept = default;

    SparseVector& operator+=(const SparseVector& t_vector);
    SparseVector& operator-=(const SparseVector& t_vector);
    SparseVector& operator*=(ValueT t_scalar);
    SparseVector& operator/=(ValueT t_scalar);

    [[nodiscard]] unsigned int size() const { return m_indices.size(); }

    [[nodiscard]] const IndexT& index_at(unsigned int t_index) const { return m_indices[t_index]; }

    [[nodiscard]] const ValueT& value_at(unsigned int t_index) const { return m_values[t_index]; }

    [[nodiscard]] ValueT get(const IndexT& t_index1) const;

    /**
     * Sets the value of a given index.
     * This method preserve the sorting order of the vector.
     * If the vector is reduced, this method also preserves the reduction.
     * @param t_index The index to set the value for.
     * @param t_value The value to set.
     */
    void set(const IndexT& t_index, const ValueT& t_value);

    void remove_at(unsigned int t_index);

    void remove(const IndexT& t_index);

    void push_back(const IndexT& t_index, const ValueT& t_value);

    void push_back(const std::vector<IndexT>& t_indices, const std::vector<ValueT>& t_values);

    void clear();

    void sort_by_index();

    void sort_by_value();

    bool is_sorted_by_index() const { return m_sorting_criteria == SortingCriteria::Index; }

    bool is_sorted_by_value() const { return m_sorting_criteria == SortingCriteria::Value; }

    void reduce();

    bool is_reduced() const { return m_is_reduced; }

    void reserve(unsigned int t_capacity);

    void reserve_at_least(unsigned int t_capacity);

    const std::vector<IndexT>& get_indices() const { return m_indices; }

    const std::vector<ValueT>& get_values() const { return m_values; }
};

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>::binary_operation_on_sorted_vectors(
        const idol::SparseVector<IndexT, ValueT, IndexExtractorT> &t_vec1,
        const idol::SparseVector<IndexT, ValueT, IndexExtractorT> &t_vec2,
        const std::function<ValueT(const ValueT &, const ValueT &)> &t_operation) {

    SparseVector result;

    if (IndexExtractorT()(t_vec1.m_indices.back()) < IndexExtractorT()(t_vec2.m_indices.front())) {
        /**
         * The last index of the first vector is less than the first index of the second vector.
         * We can simply concatenate the vectors.
         */
        result.reserve(t_vec1.size() + t_vec2.size());
        for (unsigned int i = 0, n = t_vec1.size() ; i < n ; ++i) {
            result.push_back(t_vec1.index_at(i), t_operation(t_vec1.value_at(i), ValueT{}));
        }
        for (unsigned int i = 0, n = t_vec2.size() ; i < n ; ++i) {
            result.push_back(t_vec2.index_at(i), t_operation(ValueT{}, t_vec2.value_at(i)));
        }
        return result;
    }

    if (IndexExtractorT()(t_vec2.m_indices.back()) < IndexExtractorT()(t_vec1.m_indices.front())) {
        /**
         * The last index of the second vector is less than the first index of the first vector.
         * We can simply concatenate the vectors.
         */
        result.reserve(t_vec1.size() + t_vec2.size());
        for (unsigned int i = 0, n = t_vec2.size() ; i < n ; ++i) {
            result.push_back(t_vec2.index_at(i), t_operation(ValueT{}, t_vec2.value_at(i)));
        }
        for (unsigned int i = 0, n = t_vec1.size() ; i < n ; ++i) {
            result.push_back(t_vec1.index_at(i), t_operation(t_vec1.value_at(i), ValueT{}));
        }
        return result;
    }

    /**
     * The vectors overlap.
     * We have to merge them.
     */

    result.reserve_at_least(t_vec1.size() + t_vec2.size());

    unsigned int i = 0, j = 0;
    const unsigned int n1 = t_vec1.size(), n2 = t_vec2.size();

    while (i < n1 && j < n2) {

        const auto& index1 = t_vec1.index_at(i);
        const auto& index2 = t_vec2.index_at(j);

        if (IndexExtractorT()(index1) < IndexExtractorT()(index2)) {
            result.push_back(index1, t_operation(t_vec1.value_at(i), ValueT{}));
            ++i;
            continue;
        }

        if (IndexExtractorT()(index2) < IndexExtractorT()(index1)) {
            result.push_back(index2, t_operation(ValueT{}, t_vec2.value_at(j)));
            ++j;
            continue;
        }

        result.push_back(index1, t_operation(t_vec1.value_at(i), t_vec2.value_at(j)));
        ++i;
        ++j;

    }

    for ( ; i < n1 ; ++i) {
        result.push_back(t_vec1.index_at(i), t_operation(t_vec1.value_at(i), ValueT{}));
    }

    for ( ; j < n2 ; ++j) {
        result.push_back(t_vec2.index_at(j), t_operation(ValueT{}, t_vec2.value_at(j)));
    }

    return result;

}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::set(const IndexT &t_index, const ValueT &t_value) {

    if (!m_is_reduced) {

        if (m_sorting_criteria == SortingCriteria::Index) {
            /*
             * The vector is not reduced but sorted by index.
             * We can use binary search to find the index.
             * If the index is not found, we insert it to preserve the sorting order.
             * If the index is found, we update the value and set all other values with the same index to 0.
             */

            const auto it = std::lower_bound(
                    m_indices.begin(),
                    m_indices.end(),
                    t_index,
                    [](const IndexT& t_index1, const IndexT& t_index2) { return IndexExtractorT()(t_index1) < IndexExtractorT()(t_index2); }
            );

            if (it == m_indices.end() || IndexExtractorT()(*it) != IndexExtractorT()(t_index)) {
                m_indices.insert(it, t_index);
                m_values.insert(m_values.begin() + (it - m_indices.begin()), t_value);
                return;
            }

            m_values[it - m_indices.begin()] = t_value;

            for (unsigned int i = it - m_indices.begin() + 1, n = m_indices.size() ; i < n ; ++i) {
                if (IndexExtractorT()(m_indices[i]) == IndexExtractorT()(t_index)) {
                    m_values[i] = 0;
                }
            }

            return;
        }

        /**
         * The vector is not reduced and not sorted by index.
         * We have to iterate over all elements to find the index.
         * If the index is found, we update the value and set all other values with the same index to 0.
         * If the index is not found, we insert it.
         */

        bool found = false;
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (IndexExtractorT()(m_indices[i]) == IndexExtractorT()(t_index)) {
                m_values[i] = found ? 0 : t_value;
                found = true;
            }
        }
        if (!found) {
            push_back(t_index, t_value);
        }
        return;
    }

    if (m_sorting_criteria != SortingCriteria::Index) {
        /**
         * The vector is reduced but not sorted by index.
         * We have to iterate over all elements to find the index.
         * If the index is found, we update the value and stop, because the vector is reduced.
         * If the index is not found, we insert it.
         */

        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (IndexExtractorT()(m_indices[i]) == IndexExtractorT()(t_index)) {
                m_values[i] = t_value;
                return;
            }
        }
        push_back(t_index, t_value);
        return;
    }

    /**
     * The vector is reduced and sorted by index.
     * We can use binary search to find the index.
     * If the index is found, we update the value.
     * If the index is not found, we insert it to preserve the sorting order.
     */

    const auto it = std::lower_bound(
            m_indices.begin(),
            m_indices.end(),
            t_index,
            [](const IndexT& t_index1, const IndexT& t_index2) { return IndexExtractorT()(t_index1) < IndexExtractorT()(t_index2); }
    );

    if (it == m_indices.end() || IndexExtractorT()(*it) != IndexExtractorT()(t_index)) {
        m_indices.insert(it, t_index);
        m_values.insert(m_values.begin() + (it - m_indices.begin()), t_value);
        return;
    }

    m_values[it - m_indices.begin()] = t_value;

}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>::SparseVector() : m_sorting_criteria(SortingCriteria::Index), m_is_reduced(true) {

}


template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::remove_at(unsigned int t_index) {
    m_indices.erase(m_indices.begin() + t_index);
    m_values.erase(m_values.begin() + t_index);
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>::SparseVector(std::vector<IndexT> t_indices,
                                                                     std::vector<ValueT> t_values,
                                                                     SortingCriteria t_sorting_criteria,
                                                                     bool t_is_reduced)
                                                                 : m_indices(std::move(t_indices)),
                                                                   m_values(std::move(t_values)),
                                                                   m_sorting_criteria(t_sorting_criteria),
                                                                   m_is_reduced(t_is_reduced) {

}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::push_back(const std::vector<IndexT> &t_indices,
                                                                    const std::vector<ValueT> &t_values) {

    const unsigned int capacity = m_indices.size() + t_indices.size();

    reserve_at_least(capacity);

    m_indices.insert(m_indices.end(), t_indices.begin(), t_indices.end());
    m_values.insert(m_values.end(), t_values.begin(), t_values.end());

    m_sorting_criteria = SortingCriteria::None;
    m_is_reduced = false;

}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::push_back(const IndexT &t_index, const ValueT &t_value) {

    // We first check if the insertion will change reduced or sorting by index
    if (!m_indices.empty()) {
        if (m_is_reduced) {

            if (m_sorting_criteria == SortingCriteria::Index) {

                const auto &last_index = m_indices.back();

                if (IndexExtractorT()(last_index) > IndexExtractorT()(t_index)) {
                    m_sorting_criteria = SortingCriteria::None;
                    m_is_reduced = false;
                } else if (IndexExtractorT()(last_index) == IndexExtractorT()(t_index)) {
                    m_is_reduced = false;
                }

            } else {
                m_is_reduced = false;
                m_sorting_criteria == SortingCriteria::None;
            }

        } else {
            if (m_sorting_criteria == SortingCriteria::Index) {
                if (IndexExtractorT()(m_indices.back()) > IndexExtractorT()(t_index)) {
                    m_sorting_criteria = SortingCriteria::None;
                }
            } else {
                m_sorting_criteria = SortingCriteria::None;
            }
        }
    }

    m_indices.emplace_back(t_index);
    m_values.emplace_back(t_value);

}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>
&idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator+=(const SparseVector &t_vector) {

    if (this == &t_vector) {
        return *this *= 2;
    }

    if (m_sorting_criteria == SortingCriteria::Index && t_vector.m_sorting_criteria == SortingCriteria::Index) {
        *this = binary_operation_on_sorted_vectors(*this, t_vector, [](const ValueT& t_value1, const ValueT& t_value2) { return t_value1 + t_value2; });
        return *this;
    }

    push_back(t_vector.m_indices, t_vector.m_values);

    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>&
idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator-=(const SparseVector &t_vector) {

    if (this == &t_vector) {
        clear();
        return *this;
    }

    const unsigned int capacity = m_indices.size() + t_vector.m_indices.size();

    reserve_at_least(capacity);

    m_indices.insert(m_indices.end(), t_vector.m_indices.begin(), t_vector.m_indices.end());
    std::for_each(t_vector.m_values.begin(), t_vector.m_values.end(), [this](const ValueT& t_value) {
        m_values.emplace_back(-t_value);
    });

    m_sorting_criteria = SortingCriteria::None;
    m_is_reduced = false;

    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT> &idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator*=(ValueT t_scalar) {

    if (t_scalar == ValueT{}) {
        clear();
        return *this;
    }

    std::for_each(m_values.begin(), m_values.end(), [t_scalar](ValueT& t_value) { t_value *= t_scalar; });

    m_is_reduced = false;

    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT> &idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator/=(ValueT t_scalar) {

    std::for_each(m_values.begin(), m_values.end(), [t_scalar](ValueT& t_value) { t_value /= t_scalar; });

    m_is_reduced = false;

    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::sort_by_index() {

    if (m_sorting_criteria == SortingCriteria::Index) {
        return;
    }

    idol::sort<IndexT, IndexExtractorT>(m_indices, m_values);

    m_sorting_criteria = SortingCriteria::Index;
}


template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::sort_by_value() {

    if (m_sorting_criteria == SortingCriteria::Value) {
        return;
    }

    idol::sort<ValueT, identity<ValueT>>(m_values, m_indices);

    m_sorting_criteria = SortingCriteria::Value;
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::reserve_at_least(unsigned int t_capacity) {

    if (m_indices.capacity() < t_capacity) {
        m_values.reserve(t_capacity);
    }

    if (m_values.capacity() < t_capacity) {
        m_values.reserve(t_capacity);
    }

}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::reserve(unsigned int t_capacity) {
    m_indices.reserve(t_capacity);
    m_values.reserve(t_capacity);
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::clear() {
    m_indices.clear();
    m_values.clear();
    m_sorting_criteria = SortingCriteria::Index;
    m_is_reduced = true;
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::reduce() {

    if (m_is_reduced) {
        return;
    }

    if (m_sorting_criteria != SortingCriteria::Index) {
        sort_by_index();
    }

    std::vector<IndexT> new_indices;
    std::vector<ValueT> new_values;

    unsigned int i = 0, current = -1;
    std::optional<IndexT> current_index;
    const unsigned int n = m_indices.size();

    for ( ; i < n ; ++i) {

        const auto& next_index = m_indices[i];

        if (current_index.has_value() && IndexExtractorT()(*current_index) == IndexExtractorT()(next_index)) {
            new_values[current] += m_values[i];
            continue;
        }

        new_indices.emplace_back(std::move(m_indices[i]));
        new_values.emplace_back(std::move(m_values[i]));

        ++current;
        current_index = new_indices[current];

    }

    m_indices = std::move(new_indices);
    m_values = std::move(new_values);

    m_is_reduced = true;

}

template<class IndexT, class ValueT, class IndexExtractorT>
ValueT idol::SparseVector<IndexT, ValueT, IndexExtractorT>::get(const IndexT &t_index) const {

    if (!m_is_reduced) {
        double result = 0.;
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (IndexExtractorT()(m_indices[i]) == IndexExtractorT()(t_index)) {
                result += m_values[i];
            }
        }
        return result;
    }

    if (m_sorting_criteria != SortingCriteria::Index) {
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (IndexExtractorT()(m_indices[i]) == IndexExtractorT()(t_index)) {
                return m_values[i];
            }
        }
        return ValueT{};
    }

    const auto it = std::lower_bound(
            m_indices.begin(),
            m_indices.end(),
            t_index,
            [](const IndexT& t_index1, const IndexT& t_index2) { return IndexExtractorT()(t_index1) < IndexExtractorT()(t_index2); }
    );

    if (it == m_indices.end() || IndexExtractorT()(*it) != IndexExtractorT()(t_index)) {
        return ValueT{};
    }

    return m_values[it - m_indices.begin()];
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::remove(const IndexT &t_index) {

    if (!m_is_reduced || m_sorting_criteria != SortingCriteria::Index) {

        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {

            if (IndexExtractorT()(m_indices[i]) != IndexExtractorT()(t_index)) {
                continue;
            }

            remove_at(i);
            --i;
            --n;

            if (m_is_reduced) {
                return;
            }

        }

        return;
    }

    const auto it = std::lower_bound(
            m_indices.begin(),
            m_indices.end(),
            t_index,
            [](const IndexT& t_index1, const IndexT& t_index2) { return IndexExtractorT()(t_index1) < IndexExtractorT()(t_index2); }
    );

    if (it == m_indices.end() || IndexExtractorT()(*it) != IndexExtractorT()(t_index)) {
        return;
    }

    remove_at(it - m_indices.begin());

}

template<class IndexT, class ValueT, class IndexExtractorT>
std::ostream &operator<<(std::ostream &t_stream, const idol::SparseVector<IndexT, ValueT, IndexExtractorT> &t_vector) {

    const auto& indices = t_vector.get_indices();
    const auto& values = t_vector.get_values();

    for (unsigned int i = 0, n = t_vector.size() ; i < n ; ++i) {
        t_stream << indices[i] << ": " << values[i] << '\n';
    }

    return t_stream;
}

#endif //IDOL_SPARSEVECTOR_H
