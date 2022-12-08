//
// Created by henri on 08/12/22.
//

#ifndef IDOL_ATTRIBUTES_ANNOTATION_H
#define IDOL_ATTRIBUTES_ANNOTATION_H

#include <limits>
#include <list>
#include "UserAttr.h"
#include "../objects/ObjectAttributes.h"

class UserAttrAttributes : public ObjectAttributes<UserAttr> {
public:
    UserAttrAttributes() = default;
    UserAttrAttributes(unsigned int t_id, unsigned int t_index) : ObjectAttributes<UserAttr>(t_id, t_index) {}

    UserAttrAttributes(const UserAttrAttributes&) = default;
    UserAttrAttributes(UserAttrAttributes&&) noexcept = default;

    UserAttrAttributes& operator=(const UserAttrAttributes&) = default;
    UserAttrAttributes& operator=(UserAttrAttributes&&) noexcept = default;
};

#endif //IDOL_ATTRIBUTES_ANNOTATION_H
