//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/matrix/Matrix.h"
#include "../../../include/modeling/models/Model.h"

void Matrix::add_row_to_columns(const Ctr &t_ctr) {
    auto& row = access_row(t_ctr);
    for (auto [var, ref] : row.linear().refs()) {
        access_column(var).linear().refs().set(t_ctr, std::move(ref));
    }

    if (!row.rhs().is_zero()) {
        auto &rhs = access_rhs();
        rhs.refs().set(t_ctr, row.impl().refs().get_constant());
    }
}

void Matrix::add_column_to_rows(const Var &t_var) {
    auto& column = access_column(t_var);
    for (auto [ctr, ref] : column.linear().refs()) {
        access_row(ctr).linear().refs().set(t_var, std::move(ref));
    }

    if (!column.obj().is_zero()) {
        auto &obj = access_obj();
        obj.linear().refs().set(t_var, column.impl().refs().get_constant());
    }
}

void Matrix::remove_row_from_columns(const Ctr &t_ctr) {
    auto& row = access_row(t_ctr);
    for (const auto& [var, ptr_to_coeff] : row.linear().refs()) {
        access_column(var).impl().linear().remove(t_ctr);
    }

    auto& rhs = access_rhs();
    rhs.remove(t_ctr);
}

void Matrix::remove_column_from_rows(const Var &t_var) {
    auto& column = access_column(t_var);
    for (const auto& [ctr, ptr_to_coeff] : column.impl().linear().refs()) {
        access_row(ctr).linear().remove(t_var);
    }

    auto& obj = access_obj();
    obj.linear().remove(t_var);
}

void Matrix::replace_objective(Expr<Var> &&t_objective) {
    auto& obj = access_obj();

    for (const auto& [var, coeff] : obj.linear()) {
        access_column(var).set_obj(0.);
    }

    obj = std::move(t_objective);

    for (auto [var, ref] : obj.linear().refs()) {
        access_column(var).impl().refs().set_constant(std::move(ref));
    }
}

void Matrix::replace_right_handside(LinExpr<Ctr> &&t_right_handside) {
    auto& rhs = access_rhs();

    for (const auto& [ctr, coeff] : rhs) {
        access_row(ctr).set_rhs(0.);
    }

    rhs = std::move(t_right_handside);

    for (auto [ctr, ref] : rhs.refs()) {
        access_row(ctr).impl().refs().set_constant(std::move(ref));
    }
}

void Matrix::add_to_obj(const Var& t_var, Constant&& t_constant) {

    auto& column = access_column(t_var);

    if (column.obj().is_zero()) {
        column.set_obj(std::move(t_constant));
        access_obj().linear().refs().set(t_var, column.impl().refs().get_constant());
        return;
    }

    if (t_constant.is_zero()) {
        access_obj().linear().remove(t_var);
    }
    column.set_obj(std::move(t_constant));

}

void Matrix::add_to_rhs(const Ctr &t_ctr, Constant &&t_constant) {

    auto& row = access_row(t_ctr);

    if (row.rhs().is_zero() && !t_constant.is_zero()) {
        row.set_rhs(std::move(t_constant));
        access_rhs().refs().set(t_ctr, row.impl().refs().get_constant());
        return;
    }

    if (t_constant.is_zero()) {
        access_rhs().remove(t_ctr);
    }

    row.set_rhs(std::move(t_constant));

}

void Matrix::update_matrix_coefficient(const Ctr &t_ctr, const Var &t_var, Constant &&t_constant) {

    if (t_constant.is_zero()) {
        access_column(t_var).impl().linear().remove(t_ctr);
        access_row(t_ctr).linear().remove(t_var);
        return;
    }

    auto& column = access_column(t_var);

    auto it = column.impl().linear().refs().find(t_ctr);

    if (it == column.impl().linear().refs().end()) {
        auto inserted = column.impl().linear().refs().emplace(t_ctr, MatrixCoefficient(std::move(t_constant)));
        access_row(t_ctr).linear().refs().emplace(t_var, (*inserted).second);
    } else {
        (*it).second.value() = std::move(t_constant);
    }

}
