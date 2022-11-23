//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_ITERATORFORWARD_H
#define OPTIMIZE_ITERATORFORWARD_H

template<class T, class IteratorT = typename T::iterator, class ConstIteratorT = typename T::const_iterator>
class IteratorForward {
    T& m_container;
public:
    IteratorForward(T& t_container) : m_container(t_container) {}

    IteratorForward(const IteratorForward& t_src) = default;
    IteratorForward(IteratorForward&&) noexcept = default;

    IteratorForward& operator=(const IteratorForward&) = delete;
    IteratorForward& operator=(IteratorForward&&) noexcept = delete;

    IteratorT begin() { return m_container.begin(); }
    IteratorT end() { return m_container.end(); }
    [[nodiscard]] ConstIteratorT begin() const { return m_container.begin(); }
    [[nodiscard]] ConstIteratorT end() const { return m_container.end(); }
    [[nodiscard]] ConstIteratorT cbegin() const { return m_container.begin(); }
    [[nodiscard]] ConstIteratorT cend() const { return m_container.end(); }

    [[nodiscard]] unsigned int size() const { return m_container.size(); }
};

template<class T, class ConstIteratorT = typename T::const_iterator>
class ConstIteratorForward {
    const T& m_container;
public:
    ConstIteratorForward(const T& t_container) : m_container(t_container) {}

    ConstIteratorForward(const ConstIteratorForward&) = default;
    ConstIteratorForward(ConstIteratorForward&&) noexcept = default;

    ConstIteratorForward& operator=(const ConstIteratorForward&) = delete;
    ConstIteratorForward& operator=(ConstIteratorForward&&) noexcept = delete;

    [[nodiscard]] ConstIteratorT begin() const { return m_container.begin(); }
    [[nodiscard]] ConstIteratorT end() const { return m_container.end(); }
    [[nodiscard]] ConstIteratorT cbegin() const { return m_container.begin(); }
    [[nodiscard]] ConstIteratorT cend() const { return m_container.end(); }

    [[nodiscard]] unsigned int size() const { return m_container.size(); }
};

#endif //OPTIMIZE_ITERATORFORWARD_H
