//
// Created by henri on 24.10.24.
//

#ifndef IDOL_SPARSEVECTOR_H
#define IDOL_SPARSEVECTOR_H

#include <vector>
#include <algorithm>
#include <optional>
#include "sort.h"
#include "idol/general/utils/exceptions/Exception.h"
#include "idol/general/numericals.h"

namespace idol {
    template<class, class, class>
    class SparseVector;
}

template<class IndexT, class ValueT, class IndexExtractorT = std::conditional_t<std::is_arithmetic_v<IndexT>, idol::identity<IndexT>, idol::get_id<IndexT>>>
class idol::SparseVector {
public:
    enum class SortingCriteria {
        Index,
        Value,
        None
    };

protected:
    using IndexRawT = typename std::invoke_result<idol::get_id<IndexT>, const IndexT&>::type;
    virtual IndexRawT get_raw_index(const IndexT& t_index) const { return IndexExtractorT()(t_index); }
private:
    std::vector<IndexT> m_indices;
    std::vector<ValueT> m_values;
    SortingCriteria m_sorting_criteria;
    bool m_is_reduced; // true if the vector does not contain duplicate indices

    SparseVector& binary_operation_on_sorted_vectors(const SparseVector& t_vec2, const std::function<ValueT(const ValueT&, const ValueT&)>& t_operation);
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

    virtual SparseVector& operator+=(const SparseVector& t_vector);
    virtual SparseVector& operator-=(const SparseVector& t_vector);
    virtual SparseVector& operator*=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar);
    virtual SparseVector& operator/=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar);
    SparseVector operator-() const;

    [[nodiscard]] unsigned int size() const { return m_indices.size(); }

    [[nodiscard]] bool empty() const { return m_indices.empty(); }

    [[nodiscard]] const IndexT& index_at(unsigned int t_index) const { return m_indices[t_index]; }

    [[nodiscard]] const ValueT& value_at(unsigned int t_index) const { return m_values[t_index]; }

    [[nodiscard]] bool has_index(const IndexT& t_index) const;

    [[nodiscard]] ValueT get(const IndexT& t_index1) const;

    [[nodiscard]] bool is_zero(double t_tolerance) const;

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

    class iterator {
        unsigned int m_index;
        SparseVector* m_vector;
    public:
        iterator(unsigned int t_index, SparseVector& t_vector) : m_index(t_index), m_vector(&t_vector) {}
        iterator& operator++() { ++m_index; return *this; }
        [[nodiscard]] bool operator==(const iterator& t_other) const { return m_index == t_other.m_index; }
        [[nodiscard]] bool operator!=(const iterator& t_other) const { return m_index != t_other.m_index; }
        [[nodiscard]] std::pair<const IndexT&, const ValueT&> operator*() const { return {m_vector->index_at(m_index), m_vector->value_at(m_index)}; }
    };

    class const_iterator {
        unsigned int m_index;
        const SparseVector *m_vector;
    public:
        const_iterator(unsigned int t_index, const SparseVector &t_vector) : m_index(t_index), m_vector(&t_vector) {}
        const_iterator& operator++() { ++m_index; return *this; }
        [[nodiscard]] bool operator==(const const_iterator& t_other) const { return m_index == t_other.m_index; }
        [[nodiscard]] bool operator!=(const const_iterator& t_other) const { return m_index != t_other.m_index; }
        [[nodiscard]] std::pair<const IndexT&, const ValueT&> operator*() const { return {m_vector->index_at(m_index), m_vector->value_at(m_index)}; }
    };

    [[nodiscard]] iterator begin() { return iterator(0, *this); }

    [[nodiscard]] iterator end() { return iterator(size(), *this); }

    [[nodiscard]] const_iterator begin() const { return const_iterator(0, *this); }

    [[nodiscard]] const_iterator end() const { return const_iterator(size(), *this); }

    [[nodiscard]] const_iterator cbegin() const { return const_iterator(0, *this); }

    [[nodiscard]] const_iterator cend() const { return const_iterator(size(), *this); }

    void sparsify();

    SparseVector& merge_without_conflict(const SparseVector& t_vec);
};

template<class IndexT, class ValueT, class IndexExtractorT>
bool idol::SparseVector<IndexT, ValueT, IndexExtractorT>::is_zero(double t_tolerance) const {
    return std::all_of(m_values.begin(), m_values.end(), [t_tolerance](const ValueT& t_value) { return ::idol::is_zero(t_value, t_tolerance); });
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT> idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator-() const {
    std::for_each(m_values.begin(), m_values.end(), [](ValueT& t_value) { t_value = -t_value; });
    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::sparsify() {

    unsigned int i = 0, j = 0;
    const unsigned int n = m_indices.size();
    while (i < n) {
        if (!::idol::is_zero(m_values[i], Tolerance::Sparsity)) {
            m_indices[j] = std::move(m_indices[i]);
            m_values[j] = std::move(m_values[i]);
            ++j;
        }
        ++i;
    }

    for (unsigned int k = j; k < n; ++k) {
        m_indices.pop_back();
        m_values.pop_back();
    }

}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT> &
idol::SparseVector<IndexT, ValueT, IndexExtractorT>::merge_without_conflict(const SparseVector &t_vec) {

    if (m_sorting_criteria != SortingCriteria::Index) {
        std::cerr << "Warning: sorting vector in order to merge without conflict" << std::endl;
        sort_by_index();
    }

    const auto operation = [](const ValueT& t_x, const ValueT& t_y) -> ValueT {
        if (std::abs(t_x) > 1e-12 && std::abs(t_y) > 1e-12) {
            throw Exception("There was a conflict while merging without conflict.");
        }
        if (std::abs(t_x) > 1e-12) {
            return t_x;
        }
        return t_y;
    };

    if (!t_vec.is_sorted_by_index()) {
        auto copy = t_vec;
        copy.sort_by_index();
        return binary_operation_on_sorted_vectors(copy, operation);
    }

    return binary_operation_on_sorted_vectors(t_vec, operation);
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT>&
idol::SparseVector<IndexT, ValueT, IndexExtractorT>::binary_operation_on_sorted_vectors(
        const idol::SparseVector<IndexT, ValueT, IndexExtractorT> &t_vec2,
        const std::function<ValueT(const ValueT &, const ValueT &)> &t_operation) {

    if (this == &t_vec2) {
        for (auto& val : m_values) {
            val = t_operation(val, val);
        }
        return *this;
    }

    if (empty()) {
        reserve(t_vec2.size());
        for (unsigned int i = 0, n = t_vec2.size() ; i < n ; ++i) {
            push_back(t_vec2.index_at(i), t_operation(ValueT{}, t_vec2.value_at(i)));
        }
        return *this;
    }

    if (t_vec2.empty()) {
        return *this;
    }

    SparseVector result;

    if (get_raw_index(m_indices.back()) < get_raw_index(t_vec2.m_indices.front())) {
        /**
         * The last index of the first vector is less than the first index of the second vector.
         * We can simply concatenate the vectors.
         */
        result.reserve(size() + t_vec2.size());
        for (unsigned int i = 0, n = size() ; i < n ; ++i) {
            result.push_back(index_at(i), t_operation(value_at(i), ValueT{}));
        }
        for (unsigned int i = 0, n = t_vec2.size() ; i < n ; ++i) {
            result.push_back(t_vec2.index_at(i), t_operation(ValueT{}, t_vec2.value_at(i)));
        }
        *this = std::move(result);
        return *this;
    }

    if (get_raw_index(t_vec2.m_indices.back()) < get_raw_index(m_indices.front())) {
        /**
         * The last index of the second vector is less than the first index of the first vector.
         * We can simply concatenate the vectors.
         */
        result.reserve(size() + t_vec2.size());
        for (unsigned int i = 0, n = t_vec2.size() ; i < n ; ++i) {
            result.push_back(t_vec2.index_at(i), t_operation(ValueT{}, t_vec2.value_at(i)));
        }
        for (unsigned int i = 0, n = size() ; i < n ; ++i) {
            result.push_back(index_at(i), t_operation(value_at(i), ValueT{}));
        }
        *this = std::move(result);
        return *this;
    }

    /**
     * The vectors overlap.
     * We have to merge them.
     */

    result.reserve_at_least(size() + t_vec2.size());

    unsigned int i = 0, j = 0;
    const unsigned int n1 = size(), n2 = t_vec2.size();

    while (i < n1 && j < n2) {

        const auto& index1 = index_at(i);
        const auto& index2 = t_vec2.index_at(j);

        if (get_raw_index(index1) < get_raw_index(index2)) {
            result.push_back(index1, t_operation(value_at(i), ValueT{}));
            ++i;
            continue;
        }

        if (get_raw_index(index2) < get_raw_index(index1)) {
            result.push_back(index2, t_operation(ValueT{}, t_vec2.value_at(j)));
            ++j;
            continue;
        }

        result.push_back(index1, t_operation(value_at(i), t_vec2.value_at(j)));
        ++i;
        ++j;

    }

    for ( ; i < n1 ; ++i) {
        result.push_back(index_at(i), t_operation(value_at(i), ValueT{}));
    }

    for ( ; j < n2 ; ++j) {
        result.push_back(t_vec2.index_at(j), t_operation(ValueT{}, t_vec2.value_at(j)));
    }

    *this = std::move(result);
    return *this;

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
                    [this](const IndexT& t_index1, const IndexT& t_index2) { return get_raw_index(t_index1) < get_raw_index(t_index2); }
            );

            if (it == m_indices.end()) {
                m_indices.emplace_back(t_index);
                m_values.emplace_back(t_value);
                return;
            }

            if (get_raw_index(*it) != get_raw_index(t_index)) {
                m_indices.insert(it, t_index);
                m_values.insert(m_values.begin() + (it - m_indices.begin()), t_value);
                return;
            }

            m_values[it - m_indices.begin()] = t_value;

            for (unsigned int i = it - m_indices.begin() + 1, n = m_indices.size() ; i < n ; ++i) {
                if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
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
            if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
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
            if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
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
            [this](const IndexT& t_index1, const IndexT& t_index2) { return get_raw_index(t_index1) < get_raw_index(t_index2); }
    );

    if (it == m_indices.end()) {
        m_indices.emplace_back(t_index);
        m_values.emplace_back(t_value);
        return;
    }

    if (get_raw_index(*it) != get_raw_index(t_index)) {
        m_values.insert(m_values.begin() + (it - m_indices.begin()), t_value);
        m_indices.insert(it, t_index);
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

                if (get_raw_index(last_index) > get_raw_index(t_index)) {
                    m_sorting_criteria = SortingCriteria::None;
                    m_is_reduced = false;
                } else if (get_raw_index(last_index) == get_raw_index(t_index)) {
                    m_is_reduced = false;
                }

            } else {
                m_is_reduced = false;
                m_sorting_criteria == SortingCriteria::None;
            }

        } else {
            if (m_sorting_criteria == SortingCriteria::Index) {
                if (get_raw_index(m_indices.back()) > get_raw_index(t_index)) {
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
/*
    if (m_sorting_criteria == SortingCriteria::Index && t_vector.m_sorting_criteria == SortingCriteria::Index) {
        return binary_operation_on_sorted_vectors(t_vector, [](const ValueT& t_value1, const ValueT& t_value2) { return t_value1 + t_value2; });;
    }
*/
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
        ValueT value = t_value;
        value *= -1.;
        m_values.emplace_back(value);
    });

    m_sorting_criteria = SortingCriteria::None;
    m_is_reduced = false;

    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT> &idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator*=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar) {

    if (::idol::is_zero(t_scalar, Tolerance::Sparsity)) {
        clear();
        return *this;
    }

    std::for_each(m_values.begin(), m_values.end(), [t_scalar](ValueT& t_value) { t_value *= t_scalar; });

    m_is_reduced = false;

    return *this;
}

template<class IndexT, class ValueT, class IndexExtractorT>
idol::SparseVector<IndexT, ValueT, IndexExtractorT> &idol::SparseVector<IndexT, ValueT, IndexExtractorT>::operator/=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar) {

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

        if (current_index.has_value() && get_raw_index(*current_index) == get_raw_index(next_index)) {
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
bool idol::SparseVector<IndexT, ValueT, IndexExtractorT>::has_index(const IndexT &t_index) const {

    if (!m_is_reduced) {
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
                return true;
            }
        }
        return false;
    }

    if (m_sorting_criteria != SortingCriteria::Index) {
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
                return true;
            }
        }
        return false;
    }

    const auto it = std::lower_bound(
            m_indices.begin(),
            m_indices.end(),
            t_index,
            [this](const IndexT& t_index1, const IndexT& t_index2) { return get_raw_index(t_index1) < get_raw_index(t_index2); }
    );

    if (it == m_indices.end() || get_raw_index(*it) != get_raw_index(t_index)) {
        return false;
    }

    return true;
}

template<class IndexT, class ValueT, class IndexExtractorT>
ValueT idol::SparseVector<IndexT, ValueT, IndexExtractorT>::get(const IndexT &t_index) const {

    if (!m_is_reduced) {
        double result = 0.;
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
                result += m_values[i];
            }
        }
        return result;
    }

    if (m_sorting_criteria != SortingCriteria::Index) {
        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {
            if (get_raw_index(m_indices[i]) == get_raw_index(t_index)) {
                return m_values[i];
            }
        }
        return ValueT{};
    }

    const auto it = std::lower_bound(
            m_indices.begin(),
            m_indices.end(),
            t_index,
            [this](const IndexT& t_index1, const IndexT& t_index2) { return get_raw_index(t_index1) < get_raw_index(t_index2); }
    );

    if (it == m_indices.end() || get_raw_index(*it) != get_raw_index(t_index)) {
        return ValueT{};
    }

    return m_values[it - m_indices.begin()];
}

template<class IndexT, class ValueT, class IndexExtractorT>
void idol::SparseVector<IndexT, ValueT, IndexExtractorT>::remove(const IndexT &t_index) {

    if (!m_is_reduced || m_sorting_criteria != SortingCriteria::Index) {

        for (unsigned int i = 0, n = m_indices.size() ; i < n ; ++i) {

            if (get_raw_index(m_indices[i]) != get_raw_index(t_index)) {
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
            [this](const IndexT& t_index1, const IndexT& t_index2) { return get_raw_index(t_index1) < get_raw_index(t_index2); }
    );

    if (it == m_indices.end() || get_raw_index(*it) != get_raw_index(t_index)) {
        return;
    }

    remove_at(it - m_indices.begin());

}

namespace idol {
    template<class IndexT, class ValueT, class IndexExtractorT>
    static  std::ostream & operator<<(std::ostream &t_stream, const idol::SparseVector<IndexT, ValueT, IndexExtractorT> &t_vector) {

        const auto &indices = t_vector.get_indices();
        const auto &values = t_vector.get_values();

        for (unsigned int i = 0, n = t_vector.size(); i < n; ++i) {
            t_stream << indices[i] << ": " << values[i] << '\n';
        }

        return t_stream;
    }

    template<class IndexT, class ValueT, class IndexExtractorT>
    SparseVector<IndexT, ValueT, IndexExtractorT> operator+(
            SparseVector<IndexT, ValueT, IndexExtractorT>&& t_x,
            const SparseVector<IndexT, ValueT, IndexExtractorT>& t_y) {
        return std::move(t_x) += t_y;
    }

    template<class IndexT, class ValueT, class IndexExtractorT>
    SparseVector<IndexT, ValueT, IndexExtractorT> operator+(
            const SparseVector<IndexT, ValueT, IndexExtractorT>& t_x,
            SparseVector<IndexT, ValueT, IndexExtractorT>&& t_y) {
        return std::move(t_y) += t_x;
    }

    template<class IndexT, class ValueT, class IndexExtractorT>
    SparseVector<IndexT, ValueT, IndexExtractorT> operator+(
            const SparseVector<IndexT, ValueT, IndexExtractorT>& t_x,
            const SparseVector<IndexT, ValueT, IndexExtractorT>& t_y) {
        return SparseVector(t_x) += t_y;
    }

    template<class IndexT, class ValueT, class IndexExtractorT>
    SparseVector<IndexT, ValueT, IndexExtractorT> operator-(
            SparseVector<IndexT, ValueT, IndexExtractorT> t_x,
            const SparseVector<IndexT, ValueT, IndexExtractorT>& t_y) {
        return std::move(t_x) -= t_y;
    }

    template<class IndexT, class ValueT, class IndexExtractorT>
    SparseVector<IndexT, ValueT, IndexExtractorT> operator+(
            SparseVector<IndexT, ValueT, IndexExtractorT> t_x,
            double t_factor) {
        return std::move(t_x) *= t_factor;
    }

    template<class IndexT, class ValueT, class IndexExtractorT>
    SparseVector<IndexT, ValueT, IndexExtractorT> operator+(
            double t_factor,
            SparseVector<IndexT, ValueT, IndexExtractorT> t_x) {
        return std::move(t_x) *= t_factor;
    }

}

#endif //IDOL_SPARSEVECTOR_H
