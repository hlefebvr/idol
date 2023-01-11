//
// Created by henri on 08/12/22.
//

#ifndef IDOL_USERATTR_H
#define IDOL_USERATTR_H

#include <any>
#include "modeling/objects/Object.h"

class UserAttr : public Object {
    friend class Model;
    friend class Env;
    const std::type_info& m_type;
    std::any m_default_value;
    template<class T> UserAttr(ObjectId&& t_ref, const T& t_default_value)
        : Object(std::move(t_ref)), m_default_value(t_default_value), m_type(typeid(T)) {}
protected:
    [[nodiscard]] bool isAnnotation() const override { return true; }
public:
    UserAttr(const UserAttr& t_annotation) = default;
    UserAttr(UserAttr&& t_annotation) noexcept = default;

    UserAttr& operator=(const UserAttr& t_annotation) = default;
    UserAttr& operator=(UserAttr&& t_annotation) noexcept = default;

    template<class T> const T& default_value() const { return std::any_cast<const T&>(m_default_value); }

    [[nodiscard]] const std::type_info& type() const { return m_type; }
};

MAKE_HASHABLE(UserAttr)

#endif //IDOL_USERATTR_H
