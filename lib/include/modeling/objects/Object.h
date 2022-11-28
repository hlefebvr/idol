//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_OBJECT_H
#define OPTIMIZE_OBJECT_H

#include "ObjectId.h"
#include <memory>

class Var;
class Ctr;

class Object {
    std::shared_ptr<ObjectId> m_object_id{};
protected:
    explicit Object(ObjectId&& t_ref) : m_object_id(std::make_shared<ObjectId>(std::move(t_ref))) {}
    [[nodiscard]] virtual bool isVar() const = 0;
    [[nodiscard]] virtual bool isCtr() const = 0;
public:
    virtual ~Object() = default;

    [[nodiscard]] unsigned int id() const { return m_object_id->id(); }

    [[nodiscard]] unsigned int index() const { return m_object_id->index(); }

    [[nodiscard]] const std::string& name() const { return m_object_id->name(); }

    bool operator==(const Object& t_rhs) const { return id() == t_rhs.id(); }

    bool operator!=(const Object& t_rhs) const { return id() != t_rhs.id(); }

    template<class T> [[nodiscard]] bool is() const {
        if constexpr (std::is_same_v<T, Var>) {
            return isVar();
        } else if (std::is_same_v<T, Ctr>) {
            return isCtr();
        } else {
            return false;
        }
    }

    template<class T> T as() const {
        return static_cast<const T&>(*this);
    }
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
