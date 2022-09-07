//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_MATRIXCOEFFICIENT_H
#define OPTIMIZE_MATRIXCOEFFICIENT_H

#include "LinExpr.h"

template<enum Player PlayerT>
class AbstractMatrixCoefficient {
public:
    virtual ~AbstractMatrixCoefficient() = default;
    virtual LinExpr<opp_player_v<PlayerT>>& value() = 0;
    void set_value(LinExpr<opp_player_v<PlayerT>>&& t_value);
};

template<enum Player PlayerT>
void AbstractMatrixCoefficient<PlayerT>::set_value(LinExpr<opp_player_v<PlayerT>> &&t_value) {
    value() = std::move(t_value);
}


template<enum Player PlayerT>
class MatrixCoefficientReference : public AbstractMatrixCoefficient<PlayerT> {
    LinExpr<opp_player_v<PlayerT>>* m_expr;
public:
    explicit MatrixCoefficientReference(AbstractMatrixCoefficient<PlayerT>& t_coef);
    LinExpr<opp_player_v<PlayerT>> &value() override;
};

template<enum Player PlayerT>
MatrixCoefficientReference<PlayerT>::MatrixCoefficientReference(AbstractMatrixCoefficient<PlayerT> &t_coef)
    : m_expr(&t_coef.value()){

}

template<enum Player PlayerT>
LinExpr<opp_player_v<PlayerT>> &MatrixCoefficientReference<PlayerT>::value() {
    return *m_expr;
}


template<enum Player PlayerT>
class MatrixCoefficient : public AbstractMatrixCoefficient<PlayerT> {
    std::unique_ptr<LinExpr<opp_player_v<PlayerT>>> m_expr;
public:
    explicit MatrixCoefficient(LinExpr<opp_player_v<PlayerT>> t_expr);
    MatrixCoefficient(MatrixCoefficient&&) noexcept = default;
    LinExpr<opp_player_v<PlayerT>> &value() override;
};

template<enum Player PlayerT>
MatrixCoefficient<PlayerT>::MatrixCoefficient(LinExpr<opp_player_v<PlayerT>> t_expr)
    : m_expr(std::make_unique<LinExpr<opp_player_v<PlayerT>>>(std::move(t_expr))) {

}

template<enum Player PlayerT>
LinExpr<opp_player_v<PlayerT>> &MatrixCoefficient<PlayerT>::value() {
    return *m_expr;
}

#endif //OPTIMIZE_MATRIXCOEFFICIENT_H
