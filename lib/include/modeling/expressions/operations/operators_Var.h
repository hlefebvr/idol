//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_VAR_H
#define IDOL_OPERATORS_VAR_H

#include "../Expr.h"

/* PRODUCT */

LinExpr<Var> operator*(double t_num, const Var& t_var);
LinExpr<Var> operator*(const Var& t_var, double t_num);
LinExpr<Var> operator*(double t_num, LinExpr<Var>&& t_lin_expr);
LinExpr<Var> operator*(LinExpr<Var>&& t_lin_expr, double t_num);
LinExpr<Var> operator*(double t_num, const LinExpr<Var>& t_lin_expr);
LinExpr<Var> operator*(const LinExpr<Var>& t_lin_expr, double t_num);
LinExpr<Var> operator*(Constant&& t_constant, const Var& t_var);
LinExpr<Var> operator*(const Var& t_var, Constant&& t_constant);
LinExpr<Var> operator*(const Constant& t_constant, const Var& t_var);
LinExpr<Var> operator*(const Var& t_var, const Constant& t_constant);

QuadExpr<Var> operator*(const Var& t_var1, const Var& t_var2);
QuadExpr<Var> operator*(const LinExpr<Var>& t_lin_expr, const Var& t_var);
QuadExpr<Var> operator*(const Var& t_var, const LinExpr<Var>& t_lin_expr);
QuadExpr<Var> operator*(double t_num, QuadExpr<Var>&& t_quad_expr);
QuadExpr<Var> operator*(double t_num, const QuadExpr<Var>& t_quad_expr);
QuadExpr<Var> operator*(QuadExpr<Var>&& t_quad_expr, double t_num);
QuadExpr<Var> operator*(const QuadExpr<Var>& t_quad_expr, double t_num);

Expr<Var> operator*(double t_num, Expr<Var>&& t_expr);
Expr<Var> operator*(double t_num, const Expr<Var>& t_expr);
Expr<Var> operator*(Expr<Var>&& t_expr, double t_num);
Expr<Var> operator*(const Expr<Var>& t_expr, double t_num);

/* ADDITION */

LinExpr<Var> operator+(LinExpr<Var>&& t_lin_expr);
LinExpr<Var> operator+(const LinExpr<Var>& t_lin_expr);

LinExpr<Var> operator+(const Var& t_a, const Var& t_b);
LinExpr<Var> operator+(LinExpr<Var>&& t_lin_expr, const Var& t_var);
LinExpr<Var> operator+(const Var& t_var, LinExpr<Var>&& t_lin_expr);
LinExpr<Var> operator+(const LinExpr<Var>& t_lin_expr, const Var& t_var);
LinExpr<Var> operator+(const Var& t_var, const LinExpr<Var>& t_lin_expr);
LinExpr<Var> operator+(LinExpr<Var>&& t_a, const LinExpr<Var>& t_b);
LinExpr<Var> operator+(const LinExpr<Var>& t_a, LinExpr<Var>&& t_b);
LinExpr<Var> operator+(LinExpr<Var>&& t_a, LinExpr<Var>&& t_b);
LinExpr<Var> operator+(const LinExpr<Var>& t_a, const LinExpr<Var>& t_b);

QuadExpr<Var> operator+(QuadExpr<Var>&& t_a, const QuadExpr<Var>& t_b);
QuadExpr<Var> operator+(const QuadExpr<Var>& t_a, QuadExpr<Var>&& t_b);
QuadExpr<Var> operator+(QuadExpr<Var>&& t_a, QuadExpr<Var>&& t_b);
QuadExpr<Var> operator+(const QuadExpr<Var>& t_a, const QuadExpr<Var>& t_b);

Expr<Var> operator+(const Expr<Var>& t_a, Expr<Var>&& t_b);
Expr<Var> operator+(Expr<Var>&& t_a, const Expr<Var>& t_b);
Expr<Var> operator+(Expr<Var>&& t_a, Expr<Var>&& t_b);
Expr<Var> operator+(const Expr<Var>& t_a, const Expr<Var>& t_b);

/* DIFFERENCE */

LinExpr<Var> operator-(const LinExpr<Var>& t_lin_expr);

LinExpr<Var> operator-(const Var& t_a, const Var& t_b);
LinExpr<Var> operator-(LinExpr<Var>&& t_lin_expr, const Var& t_var);
LinExpr<Var> operator-(const Var& t_var, const LinExpr<Var>& t_lin_expr);
LinExpr<Var> operator-(LinExpr<Var>&& t_a, const LinExpr<Var>& t_b);
LinExpr<Var> operator-(const LinExpr<Var>& t_a, const LinExpr<Var>& t_b);

QuadExpr<Var> operator-(QuadExpr<Var>&& t_a, const QuadExpr<Var>& t_b);
QuadExpr<Var> operator-(const QuadExpr<Var>& t_a, const QuadExpr<Var>& t_b);

Expr<Var> operator-(Expr<Var>&& t_a, const Expr<Var>& t_b);
Expr<Var> operator-(const Expr<Var>& t_a, const Expr<Var>& t_b);


#endif //IDOL_OPERATORS_VAR_H
