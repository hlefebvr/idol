//
// Created by henri on 15/10/22.
//
#include "../../../include/modeling/objects/Object.h"
#include "../../../include/modeling/objects/impl_Object.h"

unsigned int Object::id() const {
    return impl().id();
}

unsigned int Object::model_id() const {
    return impl().model_id();
}

const std::string &Object::name() const {
    return impl().name();
}

unsigned int Object::index() const {
    return impl().index();
}
