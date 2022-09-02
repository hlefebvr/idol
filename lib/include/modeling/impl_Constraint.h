//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_IMPL_CONSTRAINT_H
#define OPTIMIZE_IMPL_CONSTRAINT_H

#include "TempConstraint.h"

namespace impl {
    template<enum Player> class Constraint;
}

template<enum Player PlayerT>
class impl::Constraint : public impl::Object {
    Expr<PlayerT> m_expression;
    ConstraintType m_type;
protected:
    [[nodiscard]] std::string default_name() const override;
public:
    Constraint(Env& t_env, unsigned int t_index, TempConstraint<PlayerT>&& t_temp_ctr, std::string&& t_name);

    Expr<PlayerT>& expr();

    [[nodiscard]] const Expr<PlayerT>& expr() const;

    [[nodiscard]] ConstraintType type() const;
};

template<enum Player PlayerT>
impl::Constraint<PlayerT>::Constraint(Env &t_env, unsigned int t_index, TempConstraint<PlayerT> &&t_temp_ctr, std::string &&t_name)
    : Object(t_env, t_index, std::move(t_name)), m_type(t_temp_ctr.type()), m_expression(std::move(t_temp_ctr).expr()) {

}

template<enum Player PlayerT>
Expr<PlayerT> &impl::Constraint<PlayerT>::expr() {
    return m_expression;
}

template<enum Player PlayerT>
const Expr<PlayerT> &impl::Constraint<PlayerT>::expr() const {
    return m_expression;
}

template<enum Player PlayerT>
ConstraintType impl::Constraint<PlayerT>::type() const {
    return m_type;
}

template<enum Player PlayerT>
std::string impl::Constraint<PlayerT>::default_name() const {
    return "Ctr";
}

#endif //OPTIMIZE_IMPL_CONSTRAINT_H
