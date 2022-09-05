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

template class impl::Constraint<Decision>;
template class impl::Constraint<Parameter>;

#endif //OPTIMIZE_IMPL_CONSTRAINT_H
