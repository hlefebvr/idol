//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_OBJECT_H
#define OPTIMIZE_OBJECT_H

#include <string>

namespace impl {
    class ObjectManager;
}

template<class Impl>
class Object {
    friend class impl::ObjectManager;
    Impl* m_impl;
protected:
    Impl& impl() { return *m_impl; }
    const Impl& impl() const { return *m_impl; }
public:
    explicit Object(Impl* t_impl) : m_impl(t_impl) {}

    [[nodiscard]] unsigned int id() const { return impl().id(); }

    [[nodiscard]] const std::string& name() const { return impl().name(); }

    using impl_t = Impl;
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
};


#endif //OPTIMIZE_OBJECT_H
