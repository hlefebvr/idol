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

template<enum Player PlayerT>
Constraint<PlayerT>::Constraint(impl::Constraint<PlayerT> *t_impl) : Object<impl::Constraint<PlayerT>>(t_impl) {

}

template<enum Player PlayerT>
const Expr<PlayerT> &Constraint<PlayerT>::expr() const {
    return this->impl().expr();
}

template<enum Player PlayerT>
ConstraintType Constraint<PlayerT>::type() const {
    return this->impl().type();
}

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

#endif //OPTIMIZE_CONSTRAINT_H
