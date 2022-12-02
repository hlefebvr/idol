//
// Created by henri on 17/11/22.
//

#ifndef IDOL_OPERATORS_UTILS_H
#define IDOL_OPERATORS_UTILS_H

template<class NumT>
class Range {
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
        iterator& operator++() { ++m_index; return *this; }
        iterator operator++(int) & { auto result = *this; ++m_index; return *this; }
        NumT operator*() const { return m_index; }
        NumT* operator->() const { return &m_index; }
    };

    [[nodiscard]] iterator begin() const { return iterator(m_begin); }
    [[nodiscard]] iterator end() const { return iterator(m_end); }
};

#define idol_Sum(iterator_name, iterator, expr) \
[&]() {                                         \
    Expr _idol_result;                                \
    auto _idol_iterator = iterator;                                            \
    for (auto _idol_iterator_begin = _idol_iterator.begin(), _idol_iterator_end = _idol_iterator.end() ; \
         _idol_iterator_begin != _idol_iterator_end ;  \
         ++_idol_iterator_begin) {                         \
        decltype(*_idol_iterator_begin) iterator_name = *_idol_iterator_begin;                                        \
        _idol_result += expr;                                            \
    }                                           \
    return _idol_result;                                            \
}()

#endif //IDOL_OPERATORS_UTILS_H
