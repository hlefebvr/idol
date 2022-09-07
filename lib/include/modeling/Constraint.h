//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_CONSTRAINT_H
#define OPTIMIZE_CONSTRAINT_H

#include "impl_Constraint.h"

template<enum Player PlayerT>
class Constraint : public Object<impl::Constraint<PlayerT>> {
public:
    explicit Constraint(impl::Constraint<PlayerT>* t_impl);

    Constraint(const Constraint&) = default;
    Constraint(Constraint&&) noexcept = default;

    Constraint& operator=(const Constraint&) = default;
    Constraint& operator=(Constraint&&) noexcept = default;

    [[nodiscard]] const Expr<PlayerT>& expr() const;

    [[nodiscard]] ConstraintType type() const;
};

template class Constraint<Decision>;
template class Constraint<Parameter>;

template<enum Player PlayerT>
std::ostream &operator<<(std::ostream& t_os, const Constraint<PlayerT>& t_ctr) {
    t_os << t_ctr.name() << " : ";
    t_os << t_ctr.expr();
    switch (t_ctr.type()) {
        case Equal: t_os << " == "; break;
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
    }
    return t_os << '0';
}


template<enum Player PlayerT>
struct std::hash<Constraint<PlayerT>> {
    std::size_t operator()(const Constraint<PlayerT>& t_constraint) const {
        return std::hash<unsigned int>()(t_constraint.id());
    }
};

template<enum Player PlayerT>
struct std::equal_to<Constraint<PlayerT>> {
    std::size_t operator()(const Constraint<PlayerT>& t_a, const Constraint<PlayerT>& t_b) const {
        return t_a.id() == t_b.id();
    }
};


#endif //OPTIMIZE_CONSTRAINT_H
