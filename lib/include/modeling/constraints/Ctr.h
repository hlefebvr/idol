//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTRAINT_H
#define OPTIMIZE_CONSTRAINT_H

#include "../objects/Object.h"
#include "../Types.h"
#include <string>
#include <iostream>
#include <memory>

namespace impl {
    class Ctr;
}

class Row;
class Constant;
class Var;
class Model;

namespace Solution {
    class Primal;
}

/**
 * Constraint object.
 *
 * A constraint always belong to a single Model. Note that you should be creating a constraint using the Model::add_ctr method, rather than a constructor.
 */
class Ctr : public Object {
    friend class impl::ObjectManager;
    std::weak_ptr<impl::Ctr> m_impl;

    impl::Object &impl() override;
    [[nodiscard]] const impl::Object &impl() const override;

    explicit Ctr(const std::shared_ptr<impl::Ctr>& t_impl);
public:

    Ctr(const Ctr& t_var) = default;
    Ctr(Ctr&& t_var) noexcept = default;

    Ctr& operator=(const Ctr& t_var) = default;
    Ctr& operator=(Ctr&& t_var) noexcept = default;

private:
    friend Model;
    /**
     * Returns the current type of the constraint (see also CtrType).
     * It may be modified by calling the Model::update_var_type method.
     */
    [[nodiscard]] CtrType type() const;

    /**
     * Returns the current right handside of the constraint.
     * It may be modified by calling the Model::update_rhs_coeff method.
     */
    [[nodiscard]] const Constant& rhs() const;

    /**
     * Returns the coefficient of the variable given as parameter in the constraint.
     * @param t_var The queried variable.
     */
    [[nodiscard]] const Constant& get(const Var& t_var) const;

    /**
     * Returns the current row associated to the constraint.
     * It may be modified by calling the Model::update_matrix_coeff method.
     * @return
     */
    [[nodiscard]] const Row& row() const;

    /**
     * Returns true if the constraint is violated by the solution given as parameter.
     * @param t_solution The solution to be checked for feasibility.
     */
    [[nodiscard]] bool is_violated(const Solution::Primal& t_solution) const;

public:
    [[nodiscard]] ObjectStatus status() const override;

    /**
     * The underlying implementation class. See also [PIMPL idiom](https://en.cppreference.com/w/cpp/language/pimpl).
     */
    using impl_t = impl::Ctr;
};

std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr);

MAKE_HASHABLE(Ctr)

#endif //OPTIMIZE_CONSTRAINT_H
