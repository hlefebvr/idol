//
// Created by henri on 10.11.24.
//

#ifndef IDOL_SPARSEVECTORTSL_H
#define IDOL_SPARSEVECTORTSL_H

#include "idol/general/utils/exceptions/Exception.h"
#include "idol/general/numericals.h"

namespace idol {
    template<class, class>
    class SparseVectorTSL;
}

#ifdef IDOL_USE_ROBINHOOD
#include <robin_hood.h>
#define IDOL_REF_VALUE(t_iterator) t_iterator->second
#else
#ifdef IDOL_USE_TSL
#include <tsl/sparse_map.h>
#define IDOL_REF_VALUE(t_iterator) t_iterator.value()
#else
#include <map>
#define IDOL_REF_VALUE(t_iterator) t_iterator->second
#endif
#endif

template<class IndexT, class ValueT>
class idol::SparseVectorTSL {
private:
#ifdef IDOL_USE_ROBINHOOD
    using map_t = robin_hood::unordered_map<IndexT, ValueT>;
#else
#ifdef IDOL_USE_TSL
    using map_t = tsl::sparse_map<IndexT, ValueT>;
#else
    using map_t = std::map<IndexT, ValueT>;
#endif
#endif
    map_t m_map;
public:
    SparseVectorTSL() = default;

    SparseVectorTSL(const IndexT& t_index, const ValueT& t_value) : m_map({ { t_index, t_value} }) {}

    SparseVectorTSL(const SparseVectorTSL&) = default;
    SparseVectorTSL(SparseVectorTSL&&) = default;

    SparseVectorTSL& operator=(const SparseVectorTSL&) = default;
    SparseVectorTSL& operator=(SparseVectorTSL&&) noexcept = default;

    virtual SparseVectorTSL& operator+=(const SparseVectorTSL& t_vector);
    virtual SparseVectorTSL& operator-=(const SparseVectorTSL& t_vector);
    virtual SparseVectorTSL& operator*=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar);
    virtual SparseVectorTSL& operator/=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar);
    SparseVectorTSL operator-() const;

    [[nodiscard]] unsigned int size() const { return m_map.size(); }

    [[nodiscard]] bool empty() const { return m_map.empty(); }

    [[nodiscard]] bool has_index(const IndexT& t_index) const { return m_map.find(t_index) != m_map.end(); }

    [[nodiscard]] ValueT get(const IndexT& t_index1) const;

    void set(const IndexT& t_index, const ValueT& t_value);

    [[nodiscard]] bool is_zero(double t_tolerance) const;

    void remove(const IndexT& t_index) { m_map.erase(t_index); }

    void clear() { m_map.clear(); }

    void reserve(unsigned int t_capacity) {
#ifdef IDOL_USE_TSL
        m_map.reserve(t_capacity);
#endif
    }

    using iterator = typename map_t::iterator;
    using const_iterator = typename map_t::const_iterator;

    [[nodiscard]] iterator begin() { return m_map.begin(); }

    [[nodiscard]] iterator end() { return m_map.end(); }

    [[nodiscard]] const_iterator begin() const { return m_map.cbegin(); }

    [[nodiscard]] const_iterator end() const { return m_map.cend(); }

    [[nodiscard]] const_iterator cbegin() const { return m_map.cbegin(); }

    [[nodiscard]] const_iterator cend() const { return m_map.cend(); }

    SparseVectorTSL& merge_without_conflict(const SparseVectorTSL& t_vec);
};

template<class IndexT, class ValueT>
idol::SparseVectorTSL<IndexT, ValueT>
idol::SparseVectorTSL<IndexT, ValueT>::operator-() const {
    SparseVectorTSL result;

    for (const auto& [var, val] : m_map) {
        result.m_map.emplace(var, -val);
    }

    return result;
}

template<class IndexT, class ValueT>
idol::SparseVectorTSL<IndexT, ValueT> &
idol::SparseVectorTSL<IndexT, ValueT>::operator/=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar) {

    std::vector<IndexT> to_be_removed;
    to_be_removed.reserve(m_map.size());

    for (auto it = m_map.begin(), end = m_map.end() ; it != end ; ++it) {
        IDOL_REF_VALUE(it) /= t_scalar;
        if (::idol::is_zero(it->second, Tolerance::Sparsity)) {
            to_be_removed.emplace_back(it->first);
        }
    }

    for (const auto& index : to_be_removed) {
        m_map.erase(index);
    }

    return *this;
}

template<class IndexT, class ValueT>
idol::SparseVectorTSL<IndexT, ValueT> &
idol::SparseVectorTSL<IndexT, ValueT>::operator-=(const SparseVectorTSL &t_vector) {

    if (this == &t_vector) {
        m_map.clear();
        return *this;
    }

    for (const auto& [var, val] : t_vector.m_map) {
        ValueT minus_val{};
        minus_val -= val;
        auto [it, inserted] = m_map.emplace(var, std::move(minus_val));
        if (!inserted) {
            IDOL_REF_VALUE(it) += std::move(minus_val);
            if (::idol::is_zero(it->second, Tolerance::Sparsity)) {
                m_map.erase(it);
            }
        }
    }

    return *this;
}

template<class IndexT, class ValueT>
idol::SparseVectorTSL<IndexT, ValueT> &
idol::SparseVectorTSL<IndexT, ValueT>::operator*=(std::conditional_t<std::is_arithmetic_v<ValueT>, ValueT, double> t_scalar) {

    std::vector<IndexT> to_be_removed;
    to_be_removed.reserve(m_map.size());

    for (auto it = m_map.begin(), end = m_map.end() ; it != end ; ++it) {
        IDOL_REF_VALUE(it) *= t_scalar;
        if (::idol::is_zero(it->second, Tolerance::Sparsity)) {
            to_be_removed.emplace_back(it->first);
        }
    }

    for (const auto& index : to_be_removed) {
        m_map.erase(index);
    }

    return *this;
}

template<class IndexT, class ValueT>
bool idol::SparseVectorTSL<IndexT, ValueT>::is_zero(double t_tolerance) const {
    for (const auto& [var, val] : m_map) {
        if (!::idol::is_zero(val, t_tolerance)) {
            return false;
        }
    }
    return true;
}

template<class IndexT, class ValueT>
idol::SparseVectorTSL<IndexT, ValueT>&
idol::SparseVectorTSL<IndexT, ValueT>::operator+=(const SparseVectorTSL &t_vector) {

    if (this == &t_vector) {
        return operator*=(2.);
    }

    for (const auto& [var, val] : t_vector.m_map) {
        auto [it, inserted] = m_map.emplace(var, val);
        if (!inserted) {
            IDOL_REF_VALUE(it) += val;
            if (::idol::is_zero(it->second, Tolerance::Sparsity)) {
                m_map.erase(it);
            }
        }
    }

    return *this;
}

template<class IndexT, class ValueT>
idol::SparseVectorTSL<IndexT, ValueT> &
idol::SparseVectorTSL<IndexT, ValueT>::merge_without_conflict(const SparseVectorTSL &t_vec) {

    for (const auto& [var, val] : t_vec.m_map) {
        auto [it, inserted] = m_map.emplace(var, val);
        if (!inserted) {
            throw Exception("Found conflict when explicitly merging without conflict.");
        }
    }

    return *this;
}

template<class IndexT, class ValueT>
ValueT idol::SparseVectorTSL<IndexT, ValueT>::get(const IndexT &t_index1) const {

    const auto it = m_map.find(t_index1);

    if (it == m_map.end()) {
        return ValueT{};
    }

    return it->second;
}

template<class IndexT, class ValueT>
void idol::SparseVectorTSL<IndexT, ValueT>::set(const IndexT &t_index, const ValueT &t_value) {

    if (::idol::is_zero(t_value, Tolerance::Sparsity)) {
        remove(t_index);
        return;
    }

    auto [it, inserted] = m_map.emplace(t_index, t_value);

    if (!inserted) {
        IDOL_REF_VALUE(it) = t_value;
    }

}

namespace idol {

    template<class IndexT, class ValueT>
    static  std::ostream & operator<<(std::ostream &t_stream, const idol::SparseVectorTSL<IndexT, ValueT> &t_vector) {

        for (const auto& [index, value] : t_vector) {
            t_stream << index << ": " << value << '\n';
        }

        return t_stream;
    }

    template<class IndexT, class ValueT>
    SparseVectorTSL<IndexT, ValueT> operator+(SparseVectorTSL<IndexT, ValueT>&& t_x, const SparseVectorTSL<IndexT, ValueT>& t_y) {
        return std::move(t_x) += t_y;
    }

    template<class IndexT, class ValueT>
    SparseVectorTSL<IndexT, ValueT> operator+(const SparseVectorTSL<IndexT, ValueT>& t_x, SparseVectorTSL<IndexT, ValueT>&& t_y) {
        return std::move(t_y) += t_x;
    }

    template<class IndexT, class ValueT>
    SparseVectorTSL<IndexT, ValueT> operator+(const SparseVectorTSL<IndexT, ValueT>& t_x, const SparseVectorTSL<IndexT, ValueT>& t_y) {
        return SparseVector(t_x) += t_y;
    }

    template<class IndexT, class ValueT>
    SparseVectorTSL<IndexT, ValueT> operator-(SparseVectorTSL<IndexT, ValueT> t_x, const SparseVectorTSL<IndexT, ValueT>& t_y) {
        return std::move(t_x) -= t_y;
    }

    template<class IndexT, class ValueT>
    SparseVectorTSL<IndexT, ValueT> operator+(SparseVectorTSL<IndexT, ValueT> t_x, double t_factor) {
        return std::move(t_x) *= t_factor;
    }

    template<class IndexT, class ValueT>
    SparseVectorTSL<IndexT, ValueT> operator+(double t_factor, SparseVectorTSL<IndexT, ValueT> t_x) {
        return std::move(t_x) *= t_factor;
    }

}

#endif //IDOL_SPARSEVECTORTSL_H
