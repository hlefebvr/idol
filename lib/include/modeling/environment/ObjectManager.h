//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_OBJECTMANAGER_H
#define OPTIMIZE_OBJECTMANAGER_H

#include "Env.h"
#include "../objects/ObjectId.h"
#include "../objects/Object.h"
#include "default_name.h"

namespace impl {
    class ObjectManager;
}

class impl::ObjectManager {
    Env& m_env;

    std::list<std::unique_ptr<impl::Object>>::iterator create_placeholder();
protected:
    explicit ObjectManager(Env& t_env) : m_env(t_env) {}

    template<class T, class ...Args> T create(unsigned int t_model_id, std::string&& t_name, Args ...t_args);
    template<class T> typename T::impl_t& impl(const T& t_object);
    template<class T> void free(const T& t_object);
public:
    ObjectManager(const impl::ObjectManager&) = default;
    ObjectManager(impl::ObjectManager&&) = default;

    ObjectManager& operator=(const impl::ObjectManager&) = delete;
    ObjectManager& operator=(impl::ObjectManager&&) = delete;
};

template<class T, class... Args>
T impl::ObjectManager::create(unsigned int t_model_id, std::string&& t_name, Args ...t_args) {
    using impl_t = typename T::impl_t;
    ObjectId id(t_model_id, std::move(t_name), default_name_v<T>);
    auto* ptr = new impl_t(std::move(id), std::forward<Args>(t_args)...);
    return T(ptr);
}

template<class T>
typename T::impl_t& impl::ObjectManager::impl(const T &t_object) {
    return *t_object.m_impl;
}

template<class T>
void impl::ObjectManager::free(const T &t_object) {
    const auto& id = impl(t_object).object_id();
    m_env.m_objects.erase(id.m_it);
}


class ObjectManager : public impl::ObjectManager {
    friend class Model;
    friend class Matrix;
    explicit ObjectManager(Env& t_env) : impl::ObjectManager(t_env) {}
public:
    ObjectManager(const ObjectManager&) = default;
    ObjectManager(ObjectManager&&) = default;

    ObjectManager& operator=(const ObjectManager&) = delete;
    ObjectManager& operator=(ObjectManager&&) = delete;
};

#endif //OPTIMIZE_OBJECTMANAGER_H
