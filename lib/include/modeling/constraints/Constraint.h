//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_CONSTRAINT_H
#define OPTIMIZE_CONSTRAINT_H

#include "../objects/Object.h"
#include <string>
#include <iostream>

namespace impl {
    class Ctr;
}

class Row;

class Ctr {
    friend class impl::ObjectManager;
    impl::Ctr* m_impl;

    explicit Ctr(impl::Ctr* t_impl) : m_impl(t_impl) {}
public:
    [[nodiscard]] unsigned int id() const;

    [[nodiscard]] const std::string& name() const;

    [[nodiscard]] const Row& row() const;

    using impl_t = impl::Ctr;
};

static std::ostream& operator<<(std::ostream& t_os, const Ctr& t_ctr) {
    if (t_ctr.name().empty()) {
        return t_os << "Ctr(" << t_ctr.id() << ")";
    }
    return t_os << t_ctr.name();
}

MAKE_HASHABLE(Ctr)

#endif //OPTIMIZE_CONSTRAINT_H
