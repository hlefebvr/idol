//
// Created by Henri on 02/04/2026.
//

#ifndef IDOL_QUEUE_H
#define IDOL_QUEUE_H

namespace idol {
    template<class T> class Queue;
}

template<class T>
class idol::Queue {
    std::vector<std::unique_ptr<T>> m_queue;
    unsigned int m_first_index = 0;
    unsigned int m_size = 0;
public:
    Queue(unsigned int t_size);

    [[nodiscard]] unsigned int size() const { return m_size; }
    [[nodiscard]] bool empty() const { return bool(m_size == 0); }

    void push(const T& t_x);
    void clear();

    const T& front() const { return *m_queue.at(m_first_index); }
    T& front() { return *m_queue.at(m_first_index); }

    const T& at(unsigned int t_index) const { return *m_queue.at((m_first_index + m_size - 1 - t_index) % m_queue.size()); }
    T& at(unsigned int t_index) { return *m_queue.at((m_first_index + m_size - 1 - t_index) % m_queue.size()); }
};

template <class T>
idol::Queue<T>::Queue(unsigned int t_size) : m_queue(t_size) {

}

template <class T>
void idol::Queue<T>::push(const T& t_x) {

    if (m_size < m_queue.size()) {
        const unsigned int pos = (m_first_index + m_size) % m_queue.size();
        m_queue[pos] = std::make_unique<T>(t_x);
        ++m_size;
    } else {
        m_queue[m_first_index] = std::make_unique<T>(t_x);
        m_first_index = (m_first_index + 1) % m_queue.size();
    }

}

template <class T>
void idol::Queue<T>::clear() {
    m_size = 0;
    m_first_index = 0;
    m_queue = std::vector<std::unique_ptr<T>>(m_queue.size());
}

#endif //IDOL_QUEUE_H
