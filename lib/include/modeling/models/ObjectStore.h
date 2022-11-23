//
// Created by henri on 23/11/22.
//

#ifndef IDOL_OBJECTSTORE_H
#define IDOL_OBJECTSTORE_H

#include <list>
#include <vector>
#include "errors/Exception.h"
#include "containers/IteratorForward.h"

template<class T, class AttributesT>
class ObjectStore {
    std::list<unsigned int> m_free_indices;
    std::vector<AttributesT> m_attributes;
    std::list<T> m_objects;
public:
    template<class ...Args> void add(const T& t_object, Args&& ...t_attribute_args);
    void remove(const T& t_object);

    bool has(const T& t_object) const;
    AttributesT& attributes(const T& t_object);
    const AttributesT& attributes(const T& t_object) const;

    IteratorForward<std::list<T>> objects() { return { m_objects }; }
    ConstIteratorForward<std::list<T>> objects() const { return { m_objects }; }
};

template<class T, class AttributesT>
template<class... Args>
void ObjectStore<T, AttributesT>::add(const T &t_object, Args &&... t_attribute_args) {

    // Add object
    m_objects.template emplace_back(t_object);
    auto iterator = m_objects.begin()--;

    // Create attributes
    unsigned int index;
    if (m_free_indices.empty()) {
        index = m_attributes.size();
        m_attributes.template emplace_back(t_object.id(), std::move(iterator), std::forward<Args>(t_attribute_args)...);
    } else {
        index = m_free_indices.back();
        m_free_indices.pop_back();
        m_attributes.at(index) = AttributesT(t_object.id(), std::move(iterator), std::forward<Args>(t_attribute_args)...);
    }

    // Set index
    m_objects.back().set_index(index);
}

template<class T, class AttributesT>
void ObjectStore<T, AttributesT>::remove(const T &t_object) {

    auto& attr = attributes(t_object);
    m_objects.erase(attr.iterator());
    attr.reset();

}

template<class T, class AttributesT>
AttributesT &ObjectStore<T, AttributesT>::attributes(const T &t_object) {
    if (!has(t_object)) {
        throw Exception("Object not found.");
    }
    return m_attributes.at(t_object.index());
}

template<class T, class AttributesT>
const AttributesT &ObjectStore<T, AttributesT>::attributes(const T &t_object) const {
    if (!has(t_object)) {
        throw Exception("Object not found.");
    }
    return m_attributes.at(t_object.index());
}

template<class T, class AttributesT>
bool ObjectStore<T, AttributesT>::has(const T &t_object) const {
    return t_object.index() < m_attributes.size() && m_attributes.at(t_object.index()).id() == t_object.id();
}

#endif //IDOL_OBJECTSTORE_H
