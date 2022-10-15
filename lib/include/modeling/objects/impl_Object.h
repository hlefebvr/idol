//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_IMPL_OBJECT_H
#define OPTIMIZE_IMPL_OBJECT_H

#include "ObjectId.h"
#include "../Types.h"
#include <string>

class Env;
class Object;

namespace impl {
    class Object;
}

class impl::Object {
    friend class ::Object;
    ObjectId m_object_id;
    unsigned int m_index = 0;
    ObjectStatus m_status = InModel;
    unsigned int m_count = 1;
protected:
    explicit Object(ObjectId&& t_id);
public:
    virtual ~Object() = default;

    [[nodiscard]] unsigned int id() const { return m_object_id.id(); }

    [[nodiscard]] const std::string& name() const { return m_object_id.name(); }

    [[nodiscard]] ObjectId& object_id() { return m_object_id; }

    [[nodiscard]] const ObjectId& object_id() const { return m_object_id; }

    [[nodiscard]] unsigned int index() const { return m_index; }

    void set_index(unsigned int t_index) { m_index = t_index; }

    [[nodiscard]] unsigned int model_id() const { return m_object_id.model_id(); };

    void set_model_id(unsigned int t_id) { m_object_id.set_model_id(t_id); }

    void set_status(ObjectStatus t_status) { m_status = t_status; }

    [[nodiscard]] ObjectStatus status() const { return m_status; }

    unsigned int& count() { return m_count; }
};

#endif //OPTIMIZE_IMPL_OBJECT_H
