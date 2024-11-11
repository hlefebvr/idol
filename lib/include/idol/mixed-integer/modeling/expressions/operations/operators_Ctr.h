//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_CTR_H
#define IDOL_OPERATORS_CTR_H

#include "idol/mixed-integer/modeling/expressions/Expr.h"

namespace idol {

    /* PRODUCT */

    idol::LinExpr<idol::Ctr> operator*(double t_num, const idol::Ctr &t_ctr);

    idol::LinExpr<idol::Ctr> operator*(const idol::Ctr &t_ctr, double t_num);

    idol::LinExpr<idol::Ctr> operator*(double t_num, idol::LinExpr<idol::Ctr> &&t_lin_expr);

    idol::LinExpr<idol::Ctr> operator*(idol::LinExpr<idol::Ctr> &&t_lin_expr, double t_num);

    idol::LinExpr<idol::Ctr> operator*(double t_num, const idol::LinExpr<idol::Ctr> &t_lin_expr);

    idol::LinExpr<idol::Ctr> operator*(const idol::LinExpr<idol::Ctr> &t_lin_expr, double t_num);

    //idol::LinExpr<idol::Ctr> operator*(idol::Constant &&t_constant, const idol::Ctr &t_ctr);

    //idol::LinExpr<idol::Ctr> operator*(const idol::Ctr &t_ctr, idol::Constant &&t_constant);

   // idol::LinExpr<idol::Ctr> operator*(const idol::Constant &t_constant, const idol::Ctr &t_ctr);

    //idol::LinExpr<idol::Ctr> operator*(const idol::Ctr &t_ctr, const idol::Constant &t_constant);

    idol::Expr<idol::Ctr> operator*(double t_num, idol::Expr<idol::Ctr> &&t_expr);

    idol::Expr<idol::Ctr> operator*(double t_num, const idol::Expr<idol::Ctr> &t_expr);

    idol::Expr<idol::Ctr> operator*(idol::Expr<idol::Ctr> &&t_expr, double t_num);

    idol::Expr<idol::Ctr> operator*(const idol::Expr<idol::Ctr> &t_expr, double t_num);

    /* ADDITION */

    idol::LinExpr<idol::Ctr> operator+(idol::LinExpr<idol::Ctr> &&t_lin_expr);

    idol::LinExpr<idol::Ctr> operator+(const idol::LinExpr<idol::Ctr> &t_lin_expr);

    idol::LinExpr<idol::Ctr> operator+(const idol::Ctr &t_a, const idol::Ctr &t_b);

    idol::LinExpr<idol::Ctr> operator+(idol::LinExpr<idol::Ctr> &&t_lin_expr, const idol::Ctr &t_ctr);

    idol::LinExpr<idol::Ctr> operator+(const idol::Ctr &t_ctr, idol::LinExpr<idol::Ctr> &&t_lin_expr);

    idol::LinExpr<idol::Ctr> operator+(const idol::LinExpr<idol::Ctr> &t_lin_expr, const idol::Ctr &t_ctr);

    idol::LinExpr<idol::Ctr> operator+(const idol::Ctr &t_ctr, const idol::LinExpr<idol::Ctr> &t_lin_expr);

    idol::LinExpr<idol::Ctr> operator+(idol::LinExpr<idol::Ctr> &&t_a, const idol::LinExpr<idol::Ctr> &t_b);

    idol::LinExpr<idol::Ctr> operator+(const idol::LinExpr<idol::Ctr> &t_a, idol::LinExpr<idol::Ctr> &&t_b);

    idol::LinExpr<idol::Ctr> operator+(idol::LinExpr<idol::Ctr> &&t_a, idol::LinExpr<idol::Ctr> &&t_b);

    idol::LinExpr<idol::Ctr> operator+(const idol::LinExpr<idol::Ctr> &t_a, const idol::LinExpr<idol::Ctr> &t_b);

    idol::Expr<idol::Ctr> operator+(const idol::Expr<idol::Ctr> &t_a, idol::Expr<idol::Ctr> &&t_b);

    idol::Expr<idol::Ctr> operator+(idol::Expr<idol::Ctr> &&t_a, const idol::Expr<idol::Ctr> &t_b);

    idol::Expr<idol::Ctr> operator+(idol::Expr<idol::Ctr> &&t_a, idol::Expr<idol::Ctr> &&t_b);

    idol::Expr<idol::Ctr> operator+(const idol::Expr<idol::Ctr> &t_a, const idol::Expr<idol::Ctr> &t_b);

    /* DIFFERENCE */

    idol::LinExpr<idol::Ctr> operator-(const idol::LinExpr<idol::Ctr> &t_lin_expr);

    idol::LinExpr<idol::Ctr> operator-(const idol::Ctr &t_a, const idol::Ctr &t_b);

    idol::LinExpr<idol::Ctr> operator-(idol::LinExpr<idol::Ctr> &&t_lin_expr, const idol::Ctr &t_ctr);

    idol::LinExpr<idol::Ctr> operator-(const idol::Ctr &t_ctr, const idol::LinExpr<idol::Ctr> &t_lin_expr);

    idol::LinExpr<idol::Ctr> operator-(idol::LinExpr<idol::Ctr> &&t_a, const idol::LinExpr<idol::Ctr> &t_b);

    idol::LinExpr<idol::Ctr> operator-(const idol::LinExpr<idol::Ctr> &t_a, const idol::LinExpr<idol::Ctr> &t_b);

    idol::Expr<idol::Ctr> operator-(idol::Expr<idol::Ctr> &&t_a, const idol::Expr<idol::Ctr> &t_b);

    idol::Expr<idol::Ctr> operator-(const idol::Expr<idol::Ctr> &t_a, const idol::Expr<idol::Ctr> &t_b);

}

#endif //IDOL_OPERATORS_CTR_H
