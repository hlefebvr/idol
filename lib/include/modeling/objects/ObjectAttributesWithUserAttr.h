//
// Created by henri on 08/12/22.
//

#ifndef IDOL_OBJECTATTRIBUTESWITHUSERATTR_H
#define IDOL_OBJECTATTRIBUTESWITHUSERATTR_H

#include "ObjectAttributes.h"
#include "../user_attributes/UserAttr.h"
#include <any>

template<class ObjectT>
class ObjectAttributesWithUserAttr : public ObjectAttributes<ObjectT> {
    std::vector<std::any> m_annotations;
public:
    ObjectAttributesWithUserAttr() = default;

    ObjectAttributesWithUserAttr(unsigned int t_id, unsigned int t_index)
        : ObjectAttributes<ObjectT>(t_id, t_index) {}

    template<class T>
    void set_user_attribute(const UserAttr& t_annotation, const T& t_value) {
        if (m_annotations.size() <= t_annotation.index()) {
            m_annotations.resize(t_annotation.index()+1);
        }
        m_annotations.at(t_annotation.index()) = t_value;
    }

    void reset_user_attribute(const UserAttr& t_user_attr) {
        if (m_annotations.size() < t_user_attr.index()) {
            m_annotations.at(t_user_attr.index()).reset();
        }
    }

    template<class T>
    const T& get_user_attribute(const UserAttr& t_annotation) const {
        if (t_annotation.index() >= m_annotations.size() || !m_annotations.at(t_annotation.index()).has_value()) {
            return t_annotation.default_value<T>();
        }
        return std::any_cast<const T&>(m_annotations.at(t_annotation.index()));
    }
};

#endif //IDOL_OBJECTATTRIBUTESWITHUSERATTR_H
