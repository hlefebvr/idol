//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_IMPL_OBJECT_H
#define OPTIMIZE_IMPL_OBJECT_H

#include <list>
#include <memory>
#include <string>

class Env;

namespace impl {
    class Object;
}

class impl::Object {
    static unsigned int s_id;
    const unsigned int m_id = ++s_id;
    const std::string m_name;
    Env& m_env;
    std::list<impl::Object*>::iterator m_it;
public:
    explicit Object(Env& t_env, std::string&& t_name);

    Object(const Object&) = default;
    Object(Object&&) noexcept = default;

    Object& operator=(const Object&) = delete;
    Object& operator=(Object&&) noexcept = delete;

    virtual ~Object() = default;

    [[nodiscard]] inline unsigned int id() const;

    [[nodiscard]] inline const std::string& name() const;

    void free();
};

unsigned int impl::Object::id() const {
    return m_id;
}

const std::string &impl::Object::name() const {
    return m_name;
}

#endif //OPTIMIZE_IMPL_OBJECT_H
