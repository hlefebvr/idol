//
// Created by henri on 09/09/22.
//

#ifndef OPTIMIZE_ITERATORFORWARD_H
#define OPTIMIZE_ITERATORFORWARD_H

template<class T>
class IteratorForward {
    T& m_container;
public:
    explicit IteratorForward(T& t_container) : m_container(t_container) {}

    IteratorForward(const IteratorForward& t_src) = default;
    IteratorForward(IteratorForward&&) noexcept = default;

    IteratorForward& operator=(const IteratorForward&) = delete;
    IteratorForward& operator=(IteratorForward&&) noexcept = delete;

    typename T::iterator begin() { return m_container.begin(); }
    typename T::iterator end() { return m_container.end(); }
    [[nodiscard]] typename T::const_iterator begin() const { return m_container.begin(); }
    [[nodiscard]] typename T::const_iterator end() const { return m_container.end(); }
    [[nodiscard]] typename T::const_iterator cbegin() const { return m_container.begin(); }
    [[nodiscard]] typename T::const_iterator cend() const { return m_container.end(); }

    [[nodiscard]] unsigned int size() const { return m_container.size(); }
};

template<class T>
class ConstIteratorForward {
    const T& m_container;
public:
    explicit ConstIteratorForward(const T& t_container) : m_container(t_container) {}

    ConstIteratorForward(const ConstIteratorForward&) = default;
    ConstIteratorForward(ConstIteratorForward&&) noexcept = default;

    ConstIteratorForward& operator=(const ConstIteratorForward&) = delete;
    ConstIteratorForward& operator=(ConstIteratorForward&&) noexcept = delete;


    [[nodiscard]] typename T::const_iterator begin() const { return m_container.begin(); }
    [[nodiscard]] typename T::const_iterator end() const { return m_container.end(); }
    [[nodiscard]] typename T::const_iterator cbegin() const { return m_container.begin(); }
    [[nodiscard]] typename T::const_iterator cend() const { return m_container.end(); }

    [[nodiscard]] unsigned int size() const { return m_container.size(); }
};

#endif //OPTIMIZE_ITERATORFORWARD_H
