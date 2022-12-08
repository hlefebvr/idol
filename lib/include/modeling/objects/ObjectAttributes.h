//
// Created by henri on 08/12/22.
//

#ifndef IDOL_OBJECTATTRIBUTES_H
#define IDOL_OBJECTATTRIBUTES_H

#include <list>
#include <limits>
#include <vector>

class UserAttr;

template<class ObjectT>
class ObjectAttributes {
    unsigned int m_id = std::numeric_limits<unsigned int>::max();
    unsigned int m_index = std::numeric_limits<unsigned int>::max();
    typename std::list<ObjectT>::iterator m_it;
public:
    ObjectAttributes() = default;
    ObjectAttributes(unsigned int t_id, unsigned int t_index) : m_id(t_id), m_index(t_index) {}

    ObjectAttributes(const ObjectAttributes&) = default;
    ObjectAttributes(ObjectAttributes&&) noexcept = default;

    ObjectAttributes& operator=(const ObjectAttributes&) = default;
    ObjectAttributes& operator=(ObjectAttributes&&) noexcept = default;

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int index() const { return m_index; }

    void reset() { *this = ObjectAttributes(); }

    const typename std::list<ObjectT>::iterator& iterator() const { return m_it; }

    void set_iterator(typename std::list<ObjectT>::iterator&& t_it) { m_it = t_it; }
};

#endif //IDOL_OBJECTATTRIBUTES_H
