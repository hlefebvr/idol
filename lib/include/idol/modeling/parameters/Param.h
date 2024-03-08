//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_PARAM_H
#define OPTIMIZE_PARAM_H

#include "idol/modeling/objects/Object.h"
#include "idol/modeling/Types.h"
#include "idol/modeling/variables/Var.h"
#include "idol/modeling/constraints/Ctr.h"
#include <iostream>
#include <variant>

namespace idol {
    class Var;

    class Param;
}

/**
 * Parameter modeling-old object.
 *
 * This class is used to model parameters for optimization models. More precisely, it represents a variable, or a constraint,
 * from a model `A` which is considered constant in another model `B`.
 *
 * Parameters are created using the `Param` constructors or by prepending a variable, or a constraint, by the `!` symbol.
 *
 * **Example**:
 * ```cpp
 * auto parametrized_expression = 2 * !x * y; // Here, x is viewed as a constant
 * ```
 *
 */
class idol::Param {
    std::variant<Var, Ctr> m_value;
public:
    /**
     * Constructor.
     *
     * Creates a new parameter associated to a variable.
     * @param t_var The variable.
     */
    explicit Param(const Var& t_var) : m_value(t_var) {}

    /**
     * Constructor.
     *
     * Creates a new parameter associated to a constraint.
     * @param t_var The constraint.
     */
    explicit Param(const Ctr& t_ctr) : m_value(t_ctr) {}

    /**
     * Copy constructor.
     * @param t_param The parameter to copy.
     */
    Param(const Param& t_param) = default;

    /**
     * Move constructor.
     * @param t_param The parameter to move.
     */
    Param(Param&& t_param) noexcept = default;

    /**
     * Copy-assignment operator.
     * @param t_param The parameter to copy.
     * @return *this
     */
    Param& operator=(const Param& t_param) = default;

    /**
     * Move-assignment operator.
     * @param t_param The parameter to move.
     * @return *this
     */
    Param& operator=(Param&& t_param) noexcept = default;

    /**
     * Returns true if the parameter represents on object of the class `T`, false otherwise.
     * @tparam T The type to be tested.
     * @return True if the parameter represents on object of the class `T`, false otherwise.
     */
    template<class T> [[nodiscard]] bool is() const { return std::holds_alternative<T>(m_value); }

    /**
     * Returns the optimization object behind the parameter.
     *
     * **Example**:
     * ```cpp
     * Var x = model.add_var(0., 1., Continuous, "x");
     * Param param = !x;
     * Var var = param.as<Var>(); // "x"
     * ```
     * @tparam T The type of the underlying object.
     * @return The optimization object behind the parameter.
     */
    template<class T> [[nodiscard]] T as() const { return std::get<T>(m_value); }

    /**
     * Returns the id of the object behind the parameter.
     * @return The id of the object behind the parameter.
     */
    [[nodiscard]] unsigned int id() const { return is<Var>() ? as<Var>().id() : as<Ctr>().id(); }

    /**
     * Returns the name of the object behind the parameter.
     * @return The name of the object behind the parameter.
     */
    [[nodiscard]] const std::string& name() const { return is<Var>() ? as<Var>().name() : as<Ctr>().name(); }
};

static idol::Param operator!(const idol::Var& t_var) { return idol::Param(t_var); }
static idol::Param operator!(const idol::Ctr& t_ctr) { return idol::Param(t_ctr); }

namespace idol {

    static std::ostream &operator<<(std::ostream &t_os, const idol::Param &t_param) {
        return t_os << '!' << t_param.name();
    }

}

IDOL_MAKE_HASHABLE(Param)

#endif //OPTIMIZE_IMPL_PARAM_H
