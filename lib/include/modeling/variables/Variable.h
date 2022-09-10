//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_VARIABLE_H
#define OPTIMIZE_VARIABLE_H

#include "../objects/Object.h"
#include "modeling/Types.h"
#include <iostream>

namespace impl {
    class Var;
}

class Column;
class Coefficient;
class Ctr;

class Var {
    friend class impl::ObjectManager;
    impl::Var* m_impl;

    explicit Var(impl::Var* t_impl) : m_impl(t_impl) {}
public:
    [[nodiscard]] unsigned int id() const;

    [[nodiscard]] unsigned int index() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] double lb() const;

    [[nodiscard]] double ub() const;

    [[nodiscard]] VarType type() const;

    [[nodiscard]] const Coefficient& obj() const;

    [[nodiscard]] const Coefficient& get(const Ctr& t_ctr) const;

    [[nodiscard]] const Column& column() const;

    using impl_t = impl::Var;
};

static std::ostream& operator<<(std::ostream& t_os, const Var& t_var) {
    return t_os << t_var.name();
}

MAKE_HASHABLE(Var)

#endif //OPTIMIZE_VARIABLE_H
