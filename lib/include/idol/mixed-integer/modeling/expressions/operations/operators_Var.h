//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_VAR_H
#define IDOL_OPERATORS_VAR_H

#include "idol/mixed-integer/modeling/expressions/Expr.h"

namespace idol {

    /* PRODUCT */

    idol::LinExpr<idol::Var> operator*(double t_num, const idol::Var &t_var);

    idol::LinExpr<idol::Var> operator*(const idol::Var &t_var, double t_num);

    idol::LinExpr<idol::Var> operator*(double t_num, idol::LinExpr<idol::Var> &&t_lin_expr);

    idol::LinExpr<idol::Var> operator*(idol::LinExpr<idol::Var> &&t_lin_expr, double t_num);

    idol::LinExpr<idol::Var> operator*(double t_num, const idol::LinExpr<idol::Var> &t_lin_expr);

    idol::LinExpr<idol::Var> operator*(const idol::LinExpr<idol::Var> &t_lin_expr, double t_num);

    idol::LinExpr<idol::Var> operator*(double t_constant, const idol::Var &t_var);

    idol::LinExpr<idol::Var> operator*(const idol::Var &t_var, double t_constant);

    idol::LinExpr<idol::Var> operator*(double t_constant, const idol::Var &t_var);

    idol::LinExpr<idol::Var> operator*(const idol::Var &t_var, double t_constant);

    idol::QuadExpr<idol::Var> operator*(const idol::Var &t_var1, const idol::Var &t_var2);

    idol::QuadExpr<idol::Var> operator*(const idol::LinExpr<idol::Var> &t_lin_expr, const idol::Var &t_var);

    idol::QuadExpr<idol::Var> operator*(const idol::Var &t_var, const idol::LinExpr<idol::Var> &t_lin_expr);

    idol::QuadExpr<idol::Var> operator*(double t_num, idol::QuadExpr<idol::Var> &&t_quad_expr);

    idol::QuadExpr<idol::Var> operator*(double t_num, const idol::QuadExpr<idol::Var> &t_quad_expr);

    idol::QuadExpr<idol::Var> operator*(idol::QuadExpr<idol::Var> &&t_quad_expr, double t_num);

    idol::QuadExpr<idol::Var> operator*(const idol::QuadExpr<idol::Var> &t_quad_expr, double t_num);

    idol::Expr<idol::Var> operator*(double t_num, idol::Expr<idol::Var> &&t_expr);

    idol::Expr<idol::Var> operator*(double t_num, const idol::Expr<idol::Var> &t_expr);

    idol::Expr<idol::Var> operator*(idol::Expr<idol::Var> &&t_expr, double t_num);

    idol::Expr<idol::Var> operator*(const idol::Expr<idol::Var> &t_expr, double t_num);

    /* ADDITION */

    idol::LinExpr<idol::Var> operator+(idol::LinExpr<idol::Var> &&t_lin_expr);

    idol::LinExpr<idol::Var> operator+(const idol::LinExpr<idol::Var> &t_lin_expr);

    idol::LinExpr<idol::Var> operator+(const idol::Var &t_a, const idol::Var &t_b);

    idol::Expr<idol::Var> operator+(double t_term, idol::LinExpr<idol::Var> &&t_lin_expr);

    idol::Expr<idol::Var> operator+(idol::LinExpr<idol::Var> &&t_lin_expr, double t_term);

    idol::LinExpr<idol::Var> operator+(idol::LinExpr<idol::Var> &&t_lin_expr, const idol::Var &t_var);

    idol::LinExpr<idol::Var> operator+(const idol::Var &t_var, idol::LinExpr<idol::Var> &&t_lin_expr);

    idol::LinExpr<idol::Var> operator+(const idol::LinExpr<idol::Var> &t_lin_expr, const idol::Var &t_var);

    idol::LinExpr<idol::Var> operator+(const idol::Var &t_var, const idol::LinExpr<idol::Var> &t_lin_expr);

    idol::LinExpr<idol::Var> operator+(idol::LinExpr<idol::Var> &&t_a, const idol::LinExpr<idol::Var> &t_b);

    idol::LinExpr<idol::Var> operator+(const idol::LinExpr<idol::Var> &t_a, idol::LinExpr<idol::Var> &&t_b);

    idol::LinExpr<idol::Var> operator+(idol::LinExpr<idol::Var> &&t_a, idol::LinExpr<idol::Var> &&t_b);

    idol::LinExpr<idol::Var> operator+(const idol::LinExpr<idol::Var> &t_a, const idol::LinExpr<idol::Var> &t_b);

    idol::Expr<idol::Var> operator+(double t_term, idol::QuadExpr<idol::Var> &&t_quad_expr);

    idol::Expr<idol::Var> operator+(idol::QuadExpr<idol::Var> &&t_quad_expr, double t_term);

    idol::QuadExpr<idol::Var> operator+(idol::QuadExpr<idol::Var> &&t_a, const idol::QuadExpr<idol::Var> &t_b);

    idol::QuadExpr<idol::Var> operator+(const idol::QuadExpr<idol::Var> &t_a, idol::QuadExpr<idol::Var> &&t_b);

    idol::QuadExpr<idol::Var> operator+(idol::QuadExpr<idol::Var> &&t_a, idol::QuadExpr<idol::Var> &&t_b);

    idol::QuadExpr<idol::Var> operator+(const idol::QuadExpr<idol::Var> &t_a, const idol::QuadExpr<idol::Var> &t_b);

    idol::Expr<idol::Var> operator+(const idol::Expr<idol::Var> &t_a, idol::Expr<idol::Var> &&t_b);

    idol::Expr<idol::Var> operator+(idol::Expr<idol::Var> &&t_a, const idol::Expr<idol::Var> &t_b);

    idol::Expr<idol::Var> operator+(idol::Expr<idol::Var> &&t_a, idol::Expr<idol::Var> &&t_b);

    idol::Expr<idol::Var> operator+(const idol::Expr<idol::Var> &t_a, const idol::Expr<idol::Var> &t_b);

    /* DIFFERENCE */

    idol::LinExpr<idol::Var> operator-(const idol::LinExpr<idol::Var> &t_lin_expr);

    idol::LinExpr<idol::Var> operator-(const idol::Var &t_a, const idol::Var &t_b);

    idol::LinExpr<idol::Var> operator-(idol::LinExpr<idol::Var> &&t_lin_expr, const idol::Var &t_var);

    idol::LinExpr<idol::Var> operator-(const idol::Var &t_var, const idol::LinExpr<idol::Var> &t_lin_expr);

    idol::LinExpr<idol::Var> operator-(idol::LinExpr<idol::Var> &&t_a, const idol::LinExpr<idol::Var> &t_b);

    idol::LinExpr<idol::Var> operator-(const idol::LinExpr<idol::Var> &t_a, const idol::LinExpr<idol::Var> &t_b);

    idol::QuadExpr<idol::Var> operator-(idol::QuadExpr<idol::Var> &&t_a, const idol::QuadExpr<idol::Var> &t_b);

    idol::QuadExpr<idol::Var> operator-(const idol::QuadExpr<idol::Var> &t_a, const idol::QuadExpr<idol::Var> &t_b);

    idol::Expr<idol::Var> operator-(idol::Expr<idol::Var> &&t_a, const idol::Expr<idol::Var> &t_b);

    idol::Expr<idol::Var> operator-(const idol::Expr<idol::Var> &t_a, const idol::Expr<idol::Var> &t_b);

}

#endif //IDOL_OPERATORS_VAR_H
