//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_PARAM_H
#define OPTIMIZE_PARAM_H

#include "../objects/Object.h"
#include "modeling/Types.h"
#include "modeling/variables/Variable.h"
#include <iostream>

class Var;

/**
 * Parameter modeling object.
 *
 * This class is used to model attributes for optimization models. More precisely, it represents a variable from a model A
 * which is considered constant in another model B.
 *
 * Parameters are created using the Model::add_parameter method, rather than by using standard constructors.
 *
 * A model cannot have a parameter referring to one of its own variables.
 */
class Param {
    Var m_variable;
public:
    Param(const Var& t_var); // NOLINT(google-explicit-constructor)

    /**
     * Returns the id of the parameter.
     */
    [[nodiscard]] unsigned int id() const;

    /**
     * Returns the variable associated to the parameter.
     */
    [[nodiscard]] const Var& variable() const;

    /**
     * Returns the name of the parameter.
     */
    [[nodiscard]] const std::string& name() const;

    Var* operator->() { return &m_variable; }

    const Var* operator->() const { return &m_variable; }
};

static Param operator!(const Var& t_var) { return t_var; }
static Var operator!(const Param& t_param) { return t_param.variable(); }

static std::ostream& operator<<(std::ostream& t_os, const Param& t_param) {
    return t_os << t_param.name();
}

MAKE_HASHABLE(Param)

#endif //OPTIMIZE_IMPL_PARAM_H
