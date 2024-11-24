//
// Created by henri on 17/11/22.
//

#ifndef IDOL_OPERATORS_UTILS_H
#define IDOL_OPERATORS_UTILS_H

namespace idol {
    template<class NumT>
    class Range;
}

template<class NumT>
class idol::Range {
    const NumT m_begin;
    const NumT m_end;
public:
    explicit Range(NumT t_end) : m_begin(0), m_end(t_end) {}
    Range(NumT t_begin, NumT t_end) : m_begin(t_begin), m_end(t_end) {}

    class iterator {
        friend class Range<NumT>;
        NumT m_index;
        explicit iterator(NumT t_index) : m_index(t_index) {}
    public:
        bool operator!=(const iterator& t_it) const { return t_it.m_index != m_index; }
        bool operator==(const iterator& t_it) const { return t_it.m_index == m_index; }
        iterator& operator++() { ++m_index; return *this; }
        iterator operator++(int) & { auto result = *this; ++m_index; return *this; }
        NumT operator*() const { return m_index; }
        NumT* operator->() const { return &m_index; }
    };

    [[nodiscard]] iterator begin() const { return iterator(m_begin); }
    [[nodiscard]] iterator end() const { return iterator(m_end); }
};

#define idol_Sum(INDEX, ITERABLE, EXPR) \
[&]() {                              \
    const auto compute_sum = [&]() { \
        const auto& __idol_iterable = ITERABLE;                                 \
        auto __idol_it = __idol_iterable.begin();      \
        auto __idol_end = __idol_iterable.end(); \
        auto INDEX = *__idol_it; \
        decltype(EXPR + EXPR) result = EXPR;          \
        for (++__idol_it ; __idol_it != __idol_end ; ++__idol_it) {                                     \
            INDEX = *__idol_it; \
            result += EXPR; \
        }                             \
        return result; \
    };                                 \
                                     \
    const auto& __idol_iterable = ITERABLE;                                 \
    auto __idol_it = __idol_iterable.begin();      \
    auto __idol_end = __idol_iterable.end();                                    \
    if (__idol_it != __idol_end) {                          \
        return compute_sum();                                 \
    }                                \
    return decltype(compute_sum()){}; \
}()


#endif //IDOL_OPERATORS_UTILS_H
