//
// Created by henri on 26/10/22.
//

#ifndef IDOL_OPERATORS_CTR_H
#define IDOL_OPERATORS_CTR_H

#include "Expr.h"

/* PRODUCT */

LinExpr<Ctr> operator*(double t_num, const Ctr& t_ctr);
LinExpr<Ctr> operator*(const Ctr& t_ctr, double t_num);
LinExpr<Ctr> operator*(double t_num, LinExpr<Ctr>&& t_lin_expr);
LinExpr<Ctr> operator*(LinExpr<Ctr>&& t_lin_expr, double t_num);
LinExpr<Ctr> operator*(double t_num, const LinExpr<Ctr>& t_lin_expr);
LinExpr<Ctr> operator*(const LinExpr<Ctr>& t_lin_expr, double t_num);
LinExpr<Ctr> operator*(Constant&& t_constant, const Ctr& t_ctr);
LinExpr<Ctr> operator*(const Ctr& t_ctr, Constant&& t_constant);
LinExpr<Ctr> operator*(const Constant& t_constant, const Ctr& t_ctr);
LinExpr<Ctr> operator*(const Ctr& t_ctr, const Constant& t_constant);

Expr<Ctr> operator*(double t_num, Expr<Ctr>&& t_expr);
Expr<Ctr> operator*(double t_num, const Expr<Ctr>& t_expr);
Expr<Ctr> operator*(Expr<Ctr>&& t_expr, double t_num);
Expr<Ctr> operator*(const Expr<Ctr>& t_expr, double t_num);

/* ADDITION */

LinExpr<Ctr> operator+(LinExpr<Ctr>&& t_lin_expr);
LinExpr<Ctr> operator+(const LinExpr<Ctr>& t_lin_expr);

Constant operator+(double t_a, Constant&& t_b);
Constant operator+(double t_a, const Constant& t_b);
Constant operator+(Constant&& t_b, double t_a);
Constant operator+(const Constant& t_b, double t_a);
Constant operator+(const Param& t_a, const Param& t_b);
Constant operator+(Constant&& t_a, const Constant& t_b);
Constant operator+(const Constant& t_a, Constant&& t_b);
Constant operator+(Constant&& t_a, Constant&& t_b);
Constant operator+(const Constant& t_a, const Constant& t_b);

LinExpr<Ctr> operator+(const Ctr& t_a, const Ctr& t_b);
LinExpr<Ctr> operator+(LinExpr<Ctr>&& t_lin_expr, const Ctr& t_ctr);
LinExpr<Ctr> operator+(const Ctr& t_ctr, LinExpr<Ctr>&& t_lin_expr);
LinExpr<Ctr> operator+(const LinExpr<Ctr>& t_lin_expr, const Ctr& t_ctr);
LinExpr<Ctr> operator+(const Ctr& t_ctr, const LinExpr<Ctr>& t_lin_expr);
LinExpr<Ctr> operator+(LinExpr<Ctr>&& t_a, const LinExpr<Ctr>& t_b);
LinExpr<Ctr> operator+(const LinExpr<Ctr>& t_a, LinExpr<Ctr>&& t_b);
LinExpr<Ctr> operator+(LinExpr<Ctr>&& t_a, LinExpr<Ctr>&& t_b);
LinExpr<Ctr> operator+(const LinExpr<Ctr>& t_a, const LinExpr<Ctr>& t_b);

Expr<Ctr> operator+(const Expr<Ctr>& t_a, Expr<Ctr>&& t_b);
Expr<Ctr> operator+(Expr<Ctr>&& t_a, const Expr<Ctr>& t_b);
Expr<Ctr> operator+(Expr<Ctr>&& t_a, Expr<Ctr>&& t_b);
Expr<Ctr> operator+(const Expr<Ctr>& t_a, const Expr<Ctr>& t_b);

/* DIFFERENCE */

LinExpr<Ctr> operator-(const LinExpr<Ctr>& t_lin_expr);

LinExpr<Ctr> operator-(const Ctr& t_a, const Ctr& t_b);
LinExpr<Ctr> operator-(LinExpr<Ctr>&& t_lin_expr, const Ctr& t_ctr);
LinExpr<Ctr> operator-(const Ctr& t_ctr, const LinExpr<Ctr>& t_lin_expr);
LinExpr<Ctr> operator-(LinExpr<Ctr>&& t_a, const LinExpr<Ctr>& t_b);
LinExpr<Ctr> operator-(const LinExpr<Ctr>& t_a, const LinExpr<Ctr>& t_b);

Expr<Ctr> operator-(Expr<Ctr>&& t_a, const Expr<Ctr>& t_b);
Expr<Ctr> operator-(const Expr<Ctr>& t_a, const Expr<Ctr>& t_b);

#endif //IDOL_OPERATORS_CTR_H
