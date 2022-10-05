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

/**
 * Parameter modeling object.
 *
 * This class is used to model parameters for optimization models. More precisely, it represents a variable from a model A
 * which is considered constant in another model B.
 *
 * Parameters are created using the Model::add_parameter method, rather than by using standard constructors.
 *
 * A model cannot have a parameter referring to one of its own variables.
 */
class Param {
    friend class impl::ObjectManager;
    impl::Param* m_impl;

    explicit Param(impl::Param* t_impl) : m_impl(t_impl) {}
public:
    /**
     * Returns the id of the parameter.
     */
    [[nodiscard]] unsigned int id() const;

    /**
     * Returns the variable associated to the parameter.
     */
    [[nodiscard]] const Var& variable() const;

    /**
     * Returns the index of the parameter in the underlying matrix.
     */
    [[nodiscard]] unsigned int index() const;

    /**
     * Returns the name of the parameter.
     */
    [[nodiscard]] const std::string& name() const;

    /**
     * The underlying implementation class. See also [PIMPL idiom](https://en.cppreference.com/w/cpp/language/pimpl).
     */
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
