//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTRAINT_H
#define OPTIMIZE_CONSTRAINT_H

#include "../objects/Object.h"
#include "modeling/Types.h"
#include <string>
#include <iostream>

namespace impl {
    class Ctr;
}

class Row;
class Coefficient;
class Var;

class Ctr {
    friend class impl::ObjectManager;
    impl::Ctr* m_impl;

    explicit Ctr(impl::Ctr* t_impl) : m_impl(t_impl) {}
public:
    [[nodiscard]] unsigned int id() const;

    [[nodiscard]] unsigned int model_id() const;

    [[nodiscard]] unsigned int index() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] CtrType type() const;

    [[nodiscard]] const Coefficient& rhs() const;

    [[nodiscard]] const Coefficient& get(const Var& t_var) const;

    [[nodiscard]] const Row& row() const;

    using impl_t = impl::Ctr;
};

std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr);

MAKE_HASHABLE(Ctr)

#endif //OPTIMIZE_CONSTRAINT_H
