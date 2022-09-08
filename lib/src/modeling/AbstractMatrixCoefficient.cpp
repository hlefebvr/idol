//
// Created by henri on 07/09/22.
//
/*
#include "modeling/MatrixCoefficient.h"
#include "modeling/LinExpr.h"
#include <memory>

template<enum Player PlayerT>
void AbstractMatrixCoefficient<PlayerT>::set_value(LinExpr<opp_player_v<PlayerT>> &&t_value) {
    value() = std::move(t_value);
}


template<enum Player PlayerT>
MatrixCoefficientReference<PlayerT>::MatrixCoefficientReference(AbstractMatrixCoefficient<PlayerT> &t_coef)
        : m_expr(&t_coef.value()){

}

template<enum Player PlayerT>
LinExpr<opp_player_v<PlayerT>> &MatrixCoefficientReference<PlayerT>::value() {
    return *m_expr;
}

template<enum Player PlayerT>
MatrixCoefficient<PlayerT>::MatrixCoefficient(LinExpr<opp_player_v<PlayerT>> t_expr)
        : m_expr(new LinExpr<opp_player_v<PlayerT>>(std::move(t_expr))) {

}

template<enum Player PlayerT>
LinExpr<opp_player_v<PlayerT>> &MatrixCoefficient<PlayerT>::value() {
    return *m_expr;
}

template<enum Player PlayerT>
MatrixCoefficient<PlayerT>::~MatrixCoefficient() {
    delete m_expr;
}
*/