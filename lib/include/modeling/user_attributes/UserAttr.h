//
// Created by henri on 08/12/22.
//

#ifndef IDOL_USERATTR_H
#define IDOL_USERATTR_H

#include <any>
#include "modeling/objects/Object.h"

class UserAttr : public Object {
    friend class Model;
    int m_default_value;
    template<class T> UserAttr(ObjectId&& t_ref, const T& t_default_value)
        : Object(std::move(t_ref)), m_default_value(t_default_value) {}
protected:
    [[nodiscard]] bool isAnnotation() const override { return true; }
public:
    UserAttr(const UserAttr& t_annotation) = default;
    UserAttr(UserAttr&& t_annotation) noexcept = default;

    UserAttr& operator=(const UserAttr& t_annotation) = default;
    UserAttr& operator=(UserAttr&& t_annotation) noexcept = default;

    int default_value() const { return m_default_value; }
};

MAKE_HASHABLE(UserAttr)

#endif //IDOL_USERATTR_H
