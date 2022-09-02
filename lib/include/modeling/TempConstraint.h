//
// Created by henri on 02/09/22.
//

#ifndef OPTIMIZE_TEMPCONSTRAINT_H
#define OPTIMIZE_TEMPCONSTRAINT_H

#include "../Types.h"
#include "Expr.h"

template<enum Player PlayerT> class Model;

template<enum Player PlayerT = Decision>
class TempConstraint {
    Expr<PlayerT> m_expression;
    ConstraintType m_type;

    TempConstraint(Expr<PlayerT>&& t_expression, ConstraintType t_type);
public:
    TempConstraint() = delete;

    TempConstraint(const TempConstraint&) = default;
    TempConstraint(TempConstraint&&) noexcept = default;

    TempConstraint& operator=(const TempConstraint&) = default;
    TempConstraint& operator=(TempConstraint&&) noexcept = default;

    template<enum Player GenPlayerT> friend TempConstraint<GenPlayerT> operator<=(Expr<GenPlayerT> t_expr, double t_rhs);
    template<enum Player GenPlayerT> friend TempConstraint<GenPlayerT> operator>=(Expr<GenPlayerT> t_expr, double t_rhs);
    template<enum Player GenPlayerT> friend TempConstraint<GenPlayerT> operator==(Expr<GenPlayerT> t_expr, double t_rhs);
    template<enum Player GenPlayerT> friend std::ostream &operator<<(std::ostream& t_os, const TempConstraint<GenPlayerT>& t_temp_ctr);

    Expr<PlayerT> expr() &&;
    [[nodiscard]] ConstraintType type() const;
};

template<enum Player PlayerT>
TempConstraint<PlayerT>::TempConstraint(Expr<PlayerT> &&t_expression, ConstraintType t_type)
    : m_expression(std::move(t_expression)), m_type(t_type) {

}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator<=(Expr<PlayerT> t_expr, double t_rhs) {
    return TempConstraint<PlayerT>(std::move(t_expr) + -1. * t_rhs, LessOrEqual);
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator<=(const LinExpr<PlayerT>& t_expr, double t_rhs) {
    return Expr<PlayerT>(t_expr) <= t_rhs;
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator<=(const Variable<PlayerT>& t_variable, double t_rhs) {
    return Expr<PlayerT>(t_variable) <= t_rhs;
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator>=(Expr<PlayerT> t_expr, double t_rhs) {
    return TempConstraint<PlayerT>(std::move(t_expr) + -1. * t_rhs, GreaterOrEqual);
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator>=(const LinExpr<PlayerT>& t_expr, double t_rhs) {
    return Expr<PlayerT>(t_expr) >= t_rhs;
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator>=(const Variable<PlayerT>& t_variable, double t_rhs) {
    return Expr<PlayerT>(t_variable) >= t_rhs;
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator==(Expr<PlayerT> t_expr, double t_rhs) {
    return TempConstraint<PlayerT>(std::move(t_expr) + -1. * t_rhs, Equal);
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator==(const LinExpr<PlayerT>& t_expr, double t_rhs) {
    return Expr<PlayerT>(t_expr) == t_rhs;
}

template<enum Player PlayerT>
TempConstraint<PlayerT> operator==(const Variable<PlayerT>& t_variable, double t_rhs) {
    return Expr<PlayerT>(t_variable) == t_rhs;
}

template<enum Player PlayerT>
Expr<PlayerT> TempConstraint<PlayerT>::expr() &&{
    return std::move(m_expression);
}

template<enum Player PlayerT>
ConstraintType TempConstraint<PlayerT>::type() const {
    return m_type;
}

template<enum Player PlayerT>
std::ostream &operator<<(std::ostream& t_os, const TempConstraint<PlayerT>& t_temp_ctr) {
    t_os << t_temp_ctr.m_expression;
    switch (t_temp_ctr.type()) {
        case Equal: t_os << " == "; break;
        case LessOrEqual: t_os << " <= "; break;
        case GreaterOrEqual: t_os << " >= "; break;
    }
    return t_os << '0';
}


#endif //OPTIMIZE_TEMPCONSTRAINT_H
