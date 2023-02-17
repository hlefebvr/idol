//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_PARAM_H
#define OPTIMIZE_PARAM_H

#include "../objects/Object.h"
#include "../Types.h"
#include "../variables/Var.h"
#include "../constraints/Ctr.h"
#include <iostream>
#include <variant>

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
class InParam {
    std::variant<Var, Ctr> m_value;
public:
    explicit InParam(const Var& t_var) : m_value(t_var) {}
    explicit InParam(const Ctr& t_ctr) : m_value(t_ctr) {}

    InParam(const InParam& t_param) = default;
    InParam(InParam&& t_param) noexcept = default;

    InParam& operator=(const InParam& t_param) = default;
    InParam& operator=(InParam&& t_param) noexcept = default;

    template<class T> [[nodiscard]] bool is() const { return std::holds_alternative<T>(m_value); }

    template<class T> [[nodiscard]] T as() const { return std::get<T>(m_value); }

    /**
     * Returns the id of the parameter.
     */
    [[nodiscard]] unsigned int id() const { return is<Var>() ? as<Var>().id() : as<Ctr>().id(); }

    /**
     * Returns the name of the parameter.
     */
    [[nodiscard]] const std::string& name() const { return is<Var>() ? as<Var>().name() : as<Ctr>().name(); }
};

static InParam operator!(const Var& t_var) { return InParam(t_var); }
static InParam operator!(const Ctr& t_ctr) { return InParam(t_ctr); }

static std::ostream& operator<<(std::ostream& t_os, const InParam& t_param) {
    return t_os << '!' << t_param.name();
}

IDOL_MAKE_HASHABLE(InParam)

#endif //OPTIMIZE_IMPL_PARAM_H
