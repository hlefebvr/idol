//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_ENV_H
#define OPTIMIZE_ENV_H

#include <memory>
#include <list>
#include "../objects/impl_Object.h"

class ObjectId;
namespace impl {
    class ObjectManager;
}

class Env {
    std::list<std::shared_ptr<impl::Object>> m_objects;

    static Env* s_unique_env;
    Env() = default;
public:
    Env(const Env&) = delete;
    Env(Env&&) noexcept = delete;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = delete;

    static Env& get();

    [[nodiscard]] unsigned int size() const { return m_objects.size(); }

    friend class impl::ObjectManager;
};

#endif //OPTIMIZE_ENV_H
