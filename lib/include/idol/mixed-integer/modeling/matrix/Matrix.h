//
// Created by henri on 04/10/22.
//

#ifndef OPTIMIZE_MATRIX_H
#define OPTIMIZE_MATRIX_H

#include <functional>
#include "idol/mixed-integer/modeling/variables/VarVersion.h"
#include "idol/mixed-integer/modeling/constraints/CtrVersion.h"

namespace idol {
    class Matrix;
}

class idol::Matrix {
    virtual Expr<Var>& access_obj() = 0;
    virtual LinExpr<Ctr>& access_rhs() = 0;
    virtual Column& access_column(const Var& t_var) = 0;
    virtual Row& access_row(const Ctr& t_ctr) = 0;
protected:
    void add_row_to_columns(const Ctr& t_ctr);
    void add_column_to_rows(const Var& t_var);
    void remove_row_from_columns(const Ctr& t_ctr);
    void remove_column_from_rows(const Var& t_var);
    void replace_objective(Expr<Var>&& t_objective);
    void replace_right_handside(LinExpr<Ctr>&& t_right_handside);
    void add_to_obj(const Var& t_var, double t_constant);
    void add_to_rhs(const Ctr& t_ctr, double t_constant);
    void update_matrix_coefficient(const Ctr &t_ctr, const Var &t_var, double t_constant);
public:
    virtual ~Matrix() = default;
};


#endif //OPTIMIZE_MATRIX_H
