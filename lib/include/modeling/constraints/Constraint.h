//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTRAINT_H
#define OPTIMIZE_CONSTRAINT_H

#include "../objects/Object.h"
#include "../Types.h"
#include <string>
#include <iostream>

namespace impl {
    class Ctr;
}

class Row;
class Constant;
class Var;

namespace Solution {
    class Primal;
}

/**
 * Constraint object.
 *
 * A constraint always belong to a single Model. Note that you should be creating a constraint using the Model::add_row method, rather than a constructor.
 */
class Ctr : public Object {
    friend class impl::ObjectManager;
    impl::Ctr* m_impl;

    impl::Object &impl() override;
    [[nodiscard]] const impl::Object &impl() const override;

    explicit Ctr(impl::Ctr* t_impl);
public:

    Ctr(const Ctr& t_var);
    Ctr(Ctr&& t_var) noexcept;

    Ctr& operator=(const Ctr& t_var);
    Ctr& operator=(Ctr&& t_var) noexcept;

    ~Ctr() override;

    /**
     * Returns the current type of the constraint (see also CtrType).
     * It may be modified by calling the Model::update_type method.
     */
    [[nodiscard]] CtrType type() const;

    /**
     * Returns the current right handside of the constraint.
     * It may be modified by calling the Model::update_coefficient_rhs method.
     */
    [[nodiscard]] const Constant& rhs() const;

    /**
     * Returns the coefficient of the variable given as parameter in the constraint.
     * @param t_var The queried variable.
     */
    [[nodiscard]] const Constant& get(const Var& t_var) const;

    /**
     * Returns the current row associated to the constraint.
     * It may be modified by calling the Model::update_coefficient method.
     * @return
     */
    [[nodiscard]] const Row& row() const;

    /**
     * Returns true if the constraint is violated by the solution given as parameter.
     * @param t_solution The solution to be checked for feasibility.
     */
    [[nodiscard]] bool is_violated(const Solution::Primal& t_solution) const;

    /**
     * The underlying implementation class. See also [PIMPL idiom](https://en.cppreference.com/w/cpp/language/pimpl).
     */
    using impl_t = impl::Ctr;
};

std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr);

MAKE_HASHABLE(Ctr)

#endif //OPTIMIZE_CONSTRAINT_H
