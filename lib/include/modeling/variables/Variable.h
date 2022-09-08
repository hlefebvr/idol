//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_VARIABLE_H
#define OPTIMIZE_VARIABLE_H

#include "../objects/Object.h"
#include <iostream>

namespace impl {
    class Var;
}

class Column;

class Var {
    friend class impl::ObjectManager;
    impl::Var* m_impl;

    explicit Var(impl::Var* t_impl) : m_impl(t_impl) {}
public:
    [[nodiscard]] unsigned int id() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] const Column& column() const;

    using impl_t = impl::Var;
};

static std::ostream& operator<<(std::ostream& t_os, const Var& t_var) {
    if (t_var.name().empty()) {
        return t_os << "Var(" << t_var.id() << ")";
    }
    return t_os << t_var.name();
}

MAKE_HASHABLE(Var)

#endif //OPTIMIZE_VARIABLE_H
