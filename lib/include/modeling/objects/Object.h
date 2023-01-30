//
// Created by henri on 27/01/23.
//

#ifndef IDOL_OBJECT_H
#define IDOL_OBJECT_H

#include <memory>
#include "ObjectId.h"

class Model;

template<class T>
class Object {
    std::shared_ptr<ObjectId<T>> m_object_id;
protected:
    [[nodiscard]] auto& versions() { return m_object_id->versions(); }

    [[nodiscard]] const auto& versions() const { return m_object_id->versions(); }

    template<class ...ArgsT>
    void create_version(const Model& t_model, unsigned int t_index, ArgsT&& ...t_args) const {
        m_object_id->versions().create(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    void remove_version(const Model& t_model) const {
        m_object_id->versions().remove(t_model);
    }
public:
    Object(typename std::list<Versions<T>>::iterator t_it, unsigned int t_id, std::string t_name)
    : m_object_id(std::make_shared<ObjectId<T>>(t_it, t_id, std::move(t_name))) {}

    Object(const Object&) = default;
    Object(Object&&) noexcept = default;

    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) noexcept = default;

    [[nodiscard]] const std::string& name() const { return m_object_id->name(); }

    [[nodiscard]] unsigned int id() const { return m_object_id->id(); }

    bool operator==(const Object<T>& t_rhs) const { return id() == t_rhs.id(); }

    bool operator!=(const Object<T>& t_rhs) const { return id() != t_rhs.id(); }
};

template<class T>
static std::ostream& operator<<(std::ostream& t_os, const Object<T>& t_var) {
    return t_os << t_var.name();
}

#define IDOL_MAKE_HASHABLE(name) \
template<> \
struct std::hash<name> { \
    std::size_t operator()(const name& t_variable) const { \
        return std::hash<unsigned int>()(t_variable.id()); \
    } \
};                          \
\
template<> \
struct std::equal_to<name> { \
    std::size_t operator()(const name& t_a, const name& t_b) const { \
        return t_a.id() == t_b.id(); \
    } \
}; \
template<> \
struct std::less<name> { \
    std::size_t operator()(const name& t_a, const name& t_b) const { \
        return t_a.id() < t_b.id(); \
    } \
};

#endif //IDOL_OBJECT_H
