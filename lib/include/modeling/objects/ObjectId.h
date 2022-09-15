//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_OBJECTID_H
#define OPTIMIZE_OBJECTID_H

#include <list>
#include <memory>
#include <string>

class Env;

namespace impl {
    class Object;
    class ObjectManager;
}

class ObjectId {
    friend class impl::ObjectManager;
    static unsigned int s_id;

    const unsigned int m_id = ++s_id;
    const unsigned int m_model_id;
    const std::string m_name;
    const std::list<std::unique_ptr<impl::Object>>::iterator m_it;

    ObjectId(const std::list<std::unique_ptr<impl::Object>>::iterator& t_it, unsigned int t_model_id, std::string&& t_name, const std::string& t_default_name);
public:
    ObjectId(const ObjectId&) = delete;
    ObjectId(ObjectId&&) = default;

    ObjectId& operator=(const ObjectId&) = delete;
    ObjectId& operator=(ObjectId&&) noexcept = delete;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] unsigned int id() const;

    [[nodiscard]] unsigned int model_id() const;
};

#endif //OPTIMIZE_OBJECTID_H
