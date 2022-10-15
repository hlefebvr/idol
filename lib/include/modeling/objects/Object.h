//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_OBJECT_H
#define OPTIMIZE_OBJECT_H

#include <string>
#include "../Types.h"

namespace impl {
    class ObjectManager;
    class Object;
}

class Object {
    virtual impl::Object& impl() = 0;
    [[nodiscard]] virtual const impl::Object& impl() const = 0;
public:
    virtual ~Object() = default;

    [[nodiscard]] unsigned int id() const;

    [[nodiscard]] unsigned int model_id() const;

    [[nodiscard]] unsigned int index() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] ObjectStatus status() const;
};

#define MAKE_HASHABLE(name) \
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


#endif //OPTIMIZE_OBJECT_H
