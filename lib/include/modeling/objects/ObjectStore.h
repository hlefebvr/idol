//
// Created by henri on 23/11/22.
//

#ifndef IDOL_OBJECTSTORE_H
#define IDOL_OBJECTSTORE_H

#include <list>
#include <vector>
#include "../../errors/Exception.h"
#include "../../containers/IteratorForward.h"
#include "ObjectId.h"

template<class T, class AttributesT>
class ObjectStore {
    std::list<unsigned int> m_free_indices;
    std::vector<AttributesT> m_attributes;
    std::list<T> m_objects;
public:
    template<class ...Args> ObjectId add_attributes(std::string&& t_name, std::string&& t_default_name, Args&& ...t_attribute_args);
    void add_object(const T& t_object);
    void remove(const T& t_object);

    bool has(const T& t_object) const;
    AttributesT& attributes(const T& t_object);
    const AttributesT& attributes(const T& t_object) const;

    IteratorForward<std::list<T>> objects() { return { m_objects }; }
    ConstIteratorForward<std::list<T>> objects() const { return { m_objects }; }
};

template<class T, class AttributesT>
template<class... Args>
ObjectId ObjectStore<T, AttributesT>::add_attributes(std::string&& t_name, std::string&& t_default_name, Args &&... t_attribute_args) {

    const bool has_free_index = !m_free_indices.empty();
    const unsigned int index = has_free_index ? m_free_indices.front() : m_attributes.size();

    ObjectId result(index, std::move(t_name), std::move(t_default_name));

    if (has_free_index) {
        m_attributes.at(result.index()) = AttributesT(result.id(), result.index(), std::forward<Args>(t_attribute_args)...);
        m_free_indices.pop_front();
    } else {
        m_attributes.template emplace_back(result.id(), result.index(), std::forward<Args>(t_attribute_args)...);
    }

    return result;
}

template<class T, class AttributesT>
void ObjectStore<T, AttributesT>::add_object(const T &t_object) {
    m_objects.template emplace_back(t_object);
    auto iterator = --m_objects.end();
    attributes(t_object).set_iterator(std::move(iterator));
}

template<class T, class AttributesT>
void ObjectStore<T, AttributesT>::remove(const T &t_object) {

    auto& attr = attributes(t_object);
    m_objects.erase(attr.iterator());
    m_free_indices.template emplace_back(attr.index());
    attr.reset();

}

template<class T, class AttributesT>
AttributesT &ObjectStore<T, AttributesT>::attributes(const T &t_object) {
    if (!has(t_object)) {
        throw Exception("Object not found: " + t_object.name() + ".");
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
