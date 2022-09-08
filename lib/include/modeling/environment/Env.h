//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_ENV_H
#define OPTIMIZE_ENV_H

#include <memory>
#include <list>
#include "../objects/impl_Object.h"

namespace impl {
    class ObjectManager;
}

class Env {
    std::list<std::unique_ptr<impl::Object>> m_objects;
public:
    Env() = default;

    Env(const Env&) = delete;
    Env(Env&&) noexcept = default;

    Env& operator=(const Env&) = delete;
    Env& operator=(Env&&) noexcept = default;

    friend class impl::ObjectManager;
};


#endif //OPTIMIZE_ENV_H
