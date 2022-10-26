//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_VAR_H
#define IDOL_OPERATORS_VAR_H

#include "Expr.h"

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

Expr<Var> operator*(double t_num, Expr<Var>&& t_expr);
Expr<Var> operator*(double t_num, const Expr<Var>& t_expr);
Expr<Var> operator*(Expr<Var>&& t_expr, double t_num);
Expr<Var> operator*(const Expr<Var>& t_expr, double t_num);

/* ADDITION */

LinExpr<Var> operator+(LinExpr<Var>&& t_lin_expr);
LinExpr<Var> operator+(const LinExpr<Var>& t_lin_expr);

Constant operator+(double t_a, Constant&& t_b);
Constant operator+(double t_a, const Constant& t_b);
Constant operator+(Constant&& t_b, double t_a);
Constant operator+(const Constant& t_b, double t_a);
Constant operator+(const Param& t_a, const Param& t_b);
Constant operator+(Constant&& t_a, const Constant& t_b);
Constant operator+(const Constant& t_a, Constant&& t_b);
Constant operator+(Constant&& t_a, Constant&& t_b);
Constant operator+(const Constant& t_a, const Constant& t_b);

LinExpr<Var> operator+(const Var& t_a, const Var& t_b);
LinExpr<Var> operator+(LinExpr<Var>&& t_lin_expr, const Var& t_var);
LinExpr<Var> operator+(const Var& t_var, LinExpr<Var>&& t_lin_expr);
LinExpr<Var> operator+(const LinExpr<Var>& t_lin_expr, const Var& t_var);
LinExpr<Var> operator+(const Var& t_var, const LinExpr<Var>& t_lin_expr);
LinExpr<Var> operator+(LinExpr<Var>&& t_a, const LinExpr<Var>& t_b);
LinExpr<Var> operator+(const LinExpr<Var>& t_a, LinExpr<Var>&& t_b);
LinExpr<Var> operator+(LinExpr<Var>&& t_a, LinExpr<Var>&& t_b);
LinExpr<Var> operator+(const LinExpr<Var>& t_a, const LinExpr<Var>& t_b);

Expr<Var> operator+(const Expr<Var>& t_a, Expr<Var>&& t_b);
Expr<Var> operator+(Expr<Var>&& t_a, const Expr<Var>& t_b);
Expr<Var> operator+(Expr<Var>&& t_a, Expr<Var>&& t_b);
Expr<Var> operator+(const Expr<Var>& t_a, const Expr<Var>& t_b);

/* DIFFERENCE */

Constant operator-(const Constant& t_coefficient);
LinExpr<Var> operator-(const LinExpr<Var>& t_lin_expr);

Constant operator-(double t_a, Constant&& t_b);
Constant operator-(double t_a, const Constant& t_b);
Constant operator-(Constant&& t_b, double t_a);
Constant operator-(const Constant& t_b, double t_a);
Constant operator-(const Param& t_a, const Param& t_b);
Constant operator-(Constant&& t_a, const Constant& t_b);
Constant operator-(const Constant& t_a, Constant&& t_b);
Constant operator-(Constant&& t_a, Constant&& t_b);
Constant operator-(const Constant& t_a, const Constant& t_b);

LinExpr<Var> operator-(const Var& t_a, const Var& t_b);
LinExpr<Var> operator-(LinExpr<Var>&& t_lin_expr, const Var& t_var);
LinExpr<Var> operator-(const Var& t_var, const LinExpr<Var>& t_lin_expr);
LinExpr<Var> operator-(LinExpr<Var>&& t_a, const LinExpr<Var>& t_b);
LinExpr<Var> operator-(const LinExpr<Var>& t_a, const LinExpr<Var>& t_b);

Expr<Var> operator-(Expr<Var>&& t_a, const Expr<Var>& t_b);
Expr<Var> operator-(const Expr<Var>& t_a, const Expr<Var>& t_b);


#endif //IDOL_OPERATORS_VAR_H
