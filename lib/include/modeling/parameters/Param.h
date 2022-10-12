//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_PARAM_H
#define OPTIMIZE_PARAM_H

#include "../objects/Object.h"
#include "../Types.h"
#include "../variables/Variable.h"
#include "../constraints/Constraint.h"
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
class Param {
    struct Impl;
    Impl* m_impl;
public:
    explicit Param(const Var& t_var);
    explicit Param(const Ctr& t_ctr);

    Param(const Param& t_param);
    Param(Param&& t_param) noexcept = default;

    Param& operator=(const Param& t_param);
    Param& operator=(Param&& t_param) noexcept = default;

    ~Param();

    template<class T> [[nodiscard]] bool is() const;

    template<class T> [[nodiscard]] T as() const;

    /**
     * Returns the id of the parameter.
     */
    [[nodiscard]] unsigned int id() const { return is<Var>() ? as<Var>().id() : as<Ctr>().id(); }

    /**
     * Returns the model id of the parameter.
     */
    [[nodiscard]] unsigned int model_id() const { return is<Var>() ? as<Var>().model_id() : as<Ctr>().model_id(); }

    /**
     * Returns the name of the parameter.
     */
    [[nodiscard]] const std::string& name() const { return is<Var>() ? as<Var>().name() : as<Ctr>().name(); }
};

struct Param::Impl {
    std::variant<Var, Ctr> value;

    explicit Impl(const Var& t_var) : value(t_var) {}
    explicit Impl(const Ctr& t_ctr) : value(t_ctr) {}

    Impl(const Impl&) = default;
    Impl(Impl&&) noexcept = default;

    Impl& operator=(const Impl&) = default;
    Impl& operator=(Impl&&) noexcept = default;
};

template<class T>
bool Param::is() const {
    return std::holds_alternative<T>(m_impl->value); // TODO: remove dependency on C++17
}

template<class T>
T Param::as() const {
    return std::get<T>(m_impl->value);
}

static Param operator!(const Var& t_var) { return Param(t_var); }
static Param operator!(const Ctr& t_ctr) { return Param(t_ctr); }
static Var operator!(const Param& t_param) { return t_param.as<Var>(); } // TODO remove this

static std::ostream& operator<<(std::ostream& t_os, const Param& t_param) {
    return t_os << t_param.name();
}

MAKE_HASHABLE(Param)

#endif //OPTIMIZE_IMPL_PARAM_H
