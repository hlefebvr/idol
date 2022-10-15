//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_VARIABLE_H
#define OPTIMIZE_VARIABLE_H

#include "../objects/Object.h"
#include "../Types.h"
#include <iostream>

namespace impl {
    class Var;
}

class Column;
class Constant;
class Ctr;

/**
 * Decision variable object.
 *
 * A variable always belongs to a single Model. Note that you should be creating a variable using the Model::add_variable method, rather than a constructor.
 */
class Var : public Object {
    friend class impl::ObjectManager;
    impl::Var* m_impl;

    impl::Object &impl() override;
    [[nodiscard]] const impl::Object &impl() const override;

    explicit Var(impl::Var* t_impl);
public:

    Var(const Var& t_var);
    Var(Var&& t_var) noexcept;

    Var& operator=(const Var& t_var);
    Var& operator=(Var&& t_var) noexcept;

    ~Var() override;

    /**
     * Returns the current lower bound for the variable.
     * It may be modified by calling the Model::update_lb method.
     */
    [[nodiscard]] double lb() const;

    /**
     * Returns the current upper bound for the variable.
     * It may be modified by calling the Model::update_ub method.
     */
    [[nodiscard]] double ub() const;

    /**
     * Returns the current type of the variable (see also VarType).
     * It may be modified by calling the Model::update_type method.
     */
    [[nodiscard]] VarType type() const;

    /**
     * Returns the current objective coefficient of the variable.
     * It may be modified by calling the Model::update_objective method.
     */
    [[nodiscard]] const Constant& obj() const;

    /**
     * Returns the coefficient of the variable in the constraint given as argument.
     * It may be modified by calling the Model::update_coefficient method.
     * @param t_ctr The constraint in which the variable is queried.
     */
    [[nodiscard]] const Constant& get(const Ctr& t_ctr) const;

    /**
     * Returns the current column associated to the variable.
     * It may be modified by calling the Model::update_coefficient method.
     */
    [[nodiscard]] const Column& column() const;

    /**
     * The underlying implementation class. See also [PIMPL idiom](https://en.cppreference.com/w/cpp/language/pimpl).
     */
    using impl_t = impl::Var;
};

static std::ostream& operator<<(std::ostream& t_os, const Var& t_var) {
    return t_os << t_var.name();
}

MAKE_HASHABLE(Var)

#endif //OPTIMIZE_VARIABLE_H
