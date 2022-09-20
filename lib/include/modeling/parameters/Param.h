//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_PARAM_H
#define OPTIMIZE_PARAM_H

#include "../objects/Object.h"
#include "modeling/Types.h"
#include <iostream>

class Var;

namespace impl {
    class Param;
}

class Param {
    friend class impl::ObjectManager;
    impl::Param* m_impl;

    explicit Param(impl::Param* t_impl) : m_impl(t_impl) {}
public:
    [[nodiscard]] unsigned int id() const;

    const Var& variable() const;

    [[nodiscard]] unsigned int index() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] double lb() const;

    [[nodiscard]] double ub() const;

    [[nodiscard]] VarType type() const;

    using impl_t = impl::Param;
};

static std::ostream& operator<<(std::ostream& t_os, const Param& t_param) {
    if (t_param.name().empty()) {
        return t_os << "Param(" << t_param.id() << std::endl;
    }
    return t_os << t_param.name();
}

MAKE_HASHABLE(Param)

#endif //OPTIMIZE_IMPL_PARAM_H
