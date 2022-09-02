//
// Created by henri on 01/09/22.
//

#ifndef OPTIMIZE_OBJECT_H
#define OPTIMIZE_OBJECT_H

#include <ostream>

class AbstractObject {
public:
    [[nodiscard]] virtual unsigned int id() const = 0;

    [[nodiscard]] virtual const std::string& name() const = 0;

    struct compare {
        bool operator()(const AbstractObject& a, const AbstractObject& b) const {
            return a.id() < b.id();
        }

        bool operator()(const AbstractObject* a, const AbstractObject* b) const {
            return a->id() < b->id();
        }

        template<class T, class S>
        bool operator()(const AbstractObject& a, std::pair<T, S>& b) const {
            static_assert(std::is_base_of_v<AbstractObject, T>);
            return a.id() < b.first.id();
        }
    };
};

static std::ostream& operator<<(std::ostream& t_os, const AbstractObject& t_object) {
    return t_os << t_object.name();
}

template<class Impl>
class Object : public AbstractObject {
    static_assert(std::is_base_of_v<impl::Object, Impl>);

    Impl* const m_impl;
protected:
    const Impl& impl() const;
public:
    explicit Object(Impl* const t_impl) : m_impl(t_impl) {}

    [[nodiscard]] unsigned int id() const final;

    [[nodiscard]] const std::string& name() const final;
};

template<class Impl>
const Impl &Object<Impl>::impl() const {
    return *m_impl;
}

template<class Impl>
unsigned int Object<Impl>::id() const {
    return impl().id();
}

template<class Impl>
const std::string &Object<Impl>::name() const {
    return impl().name();
}

#endif //OPTIMIZE_OBJECT_H
