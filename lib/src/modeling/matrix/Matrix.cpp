//
// Created by henri on 04/10/22.
//
#include <cassert>
#include "../../../include/modeling/matrix/Matrix.h"
#include "../../../include/modeling/models/Model.h"

/*

void Matrix::update_coefficient(const Var& t_var, const Ctr& t_ctr, LinExpr<Ctr> &t_column, LinExpr<Var> &t_row, Constant &&t_coefficient) {

    if (t_coefficient.is_zero()) {
        t_column.m_map.erase(t_ctr);
        t_row.m_map.erase(t_var);
        return;
    }

    auto it = t_column.m_map.find(t_ctr);

    if (it == t_column.m_map.end()) {
        auto [inserted, success] = t_column.m_map.emplace(t_ctr, std::make_unique<MatrixCoefficient>(std::move(t_coefficient)));
        assert(success);
        auto [inserted_2, success_2] = t_row.m_map.emplace(t_var, std::make_unique<MatrixCoefficientReference>(*inserted->second));
        assert(success_2);
    } else {
        it->second->value() = std::move(t_coefficient);
    }

}

*/

void Matrix::add_row_to_columns(const Ctr &t_ctr) {
    auto& row = access_row(t_ctr);
    for (const auto& [var, ptr_to_coeff] : row.lhs().linear().m_map) {
        access_column(var).components().linear().set(t_ctr, MatrixCoefficientReference(*ptr_to_coeff));
    }

    if (!row.rhs().is_zero()) {
        auto &rhs = access_rhs();
        rhs.set(t_ctr, MatrixCoefficientReference(*row.lhs().m_constant));
    }
}

void Matrix::add_column_to_rows(const Var &t_var) {
    auto& column = access_column(t_var);
    for (const auto& [ctr, ptr_to_coeff] : column.components().linear().m_map) {
        access_row(ctr).lhs().linear().set(t_var, MatrixCoefficientReference(*ptr_to_coeff));
    }

    if (!column.objective_coefficient().is_zero()) {
        auto &obj = access_obj();
        obj.linear().set(t_var, MatrixCoefficientReference(*column.components().m_constant));
    }
}

void Matrix::remove_row_from_columns(const Ctr &t_ctr) {
    auto& row = access_row(t_ctr);
    for (const auto& [var, ptr_to_coeff] : row.lhs().linear().m_map) {
        access_column(var).components().linear().set(t_ctr, 0.);
    }

    auto& rhs = access_rhs();
    rhs.set(t_ctr, 0.);
}

void Matrix::remove_column_from_rows(const Var &t_var) {
    auto& column = access_column(t_var);
    for (const auto& [ctr, ptr_to_coeff] : column.components().linear().m_map) {
        access_row(ctr).lhs().linear().set(t_var, 0.);
    }

    auto& obj = access_obj();
    obj.linear().set(t_var, 0.);
}

void Matrix::replace_objective(Expr<Var> &&t_objective) {
    auto& obj = access_obj();

    for (const auto& [var, coeff] : obj.linear()) {
        access_column(var).set_objective_coefficient(0.);
    }

    obj = std::move(t_objective);

    for (const auto& [var, ptr_to_coeff] : obj.linear().m_map) {
        access_column(var).set_objective_coefficient(MatrixCoefficientReference(*ptr_to_coeff));
    }
}

void Matrix::replace_right_handside(LinExpr<Ctr> &&t_right_handside) {
    auto& rhs = access_rhs();

    for (const auto& [ctr, coeff] : rhs) {
        access_row(ctr).set_rhs(0.);
    }

    rhs = std::move(t_right_handside);

    for (const auto& [ctr, ptr_to_coeff] : rhs.m_map) {
        access_row(ctr).set_rhs(MatrixCoefficientReference(*ptr_to_coeff));
    }
}

void Matrix::add_to_obj(const Var& t_var, Constant&& t_constant) {

    auto& column = access_column(t_var);

    if (column.objective_coefficient().is_zero()) {
        column.set_objective_coefficient(std::move(t_constant));
        access_obj().linear().set(t_var, MatrixCoefficientReference(*column.components().m_constant));
        return;
    }

    if (t_constant.is_zero()) {
        access_obj().linear().set(t_var, 0.);
    }
    column.set_objective_coefficient(std::move(t_constant));

}

void Matrix::add_to_rhs(const Ctr &t_ctr, Constant &&t_constant) {

    auto& row = access_row(t_ctr);

    if (row.rhs().is_zero() && !t_constant.is_zero()) {
        row.set_rhs(std::move(t_constant));
        access_rhs().set(t_ctr, MatrixCoefficientReference(*row.lhs().m_constant));
        return;
    }

    if (t_constant.is_zero()) {
        access_rhs().set(t_ctr, 0.);
    }

    row.set_rhs(std::move(t_constant));

}

void Matrix::update_matrix_coefficient(const Ctr &t_ctr, const Var &t_var, Constant &&t_constant) {

    if (t_constant.is_zero()) {
        access_column(t_var).components().linear().set(t_ctr, 0.);
        access_row(t_ctr).lhs().linear().set(t_var, 0.);
        return;
    }

    auto& column = access_column(t_var);

    auto it = column.components().linear().m_map.find(t_ctr);

    if (it == column.components().linear().m_map.end()) {
        auto [inserted, success] = column.components().linear().m_map.emplace(t_ctr, std::make_unique<MatrixCoefficient>(std::move(t_constant)));
        assert(success);
        auto [inserted_2, success_2] = access_row(t_ctr).lhs().linear().m_map.emplace(t_var, std::make_unique<MatrixCoefficientReference>(*inserted->second));
        assert(success_2);
    } else {
        it->second->value() = std::move(t_constant);
    }

}
