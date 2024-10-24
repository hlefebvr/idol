//
// Created by henri on 04/10/22.
//
#include <sstream>
#include "idol/modeling/matrix/Matrix.h"
#include "idol/modeling/models/Model.h"

void idol::Matrix::add_row_to_columns(const Ctr &t_ctr) {

    auto& row = access_row(t_ctr);

    for (auto [var, ref] : row.linear().refs()) {
        access_column(var).linear().refs().set(t_ctr, std::move(ref));
    }

    for (auto [pair, ref] : row.quadratic().refs()) {
        access_column(pair.first).quadratic().refs().set({ t_ctr, pair.second }, std::move(ref));
    }
    for (auto [pair, ref] : row.quadratic().refs()) {
        if (pair.first.id() != pair.second.id()) {
            access_column(pair.second).quadratic().refs().set({t_ctr, pair.first}, std::move(ref));
        }
    }

    if (std::abs(row.rhs()) >= Tolerance::Sparsity) {
        auto &rhs = access_rhs();
        rhs.refs().set(t_ctr, row.impl().refs().get_constant());
    }
}

void idol::Matrix::add_column_to_rows(const Var &t_var) {

    auto& column = access_column(t_var);

    for (auto [ctr, ref] : column.linear().refs()) {
        access_row(ctr).linear().refs().set(t_var, std::move(ref));
    }

    for (auto [pair, ref] : column.quadratic().refs()) {
        access_row(pair.first).quadratic().refs().set({ t_var, pair.second }, std::move(ref));
    }
    for (auto [pair, ref] : column.quadratic().refs()) {
        access_column(pair.second).quadratic().refs().set({ pair.first, t_var }, std::move(ref));
    }

    if (std::abs(column.obj()) >= Tolerance::Sparsity) {
        auto &obj = access_obj();
        obj.linear().refs().set(t_var, column.impl().refs().get_constant());
    }
}

void idol::Matrix::remove_row_from_columns(const Ctr &t_ctr) {
    auto& row = access_row(t_ctr);
    for (const auto& [var, ptr_to_coeff] : row.linear().refs()) {
        access_column(var).impl().linear().remove(t_ctr);
    }
    for (const auto& [pair, ptr_to_coeff] : row.quadratic().refs()) {
        access_column(pair.first).impl().quadratic().remove({ t_ctr, pair.second });
        if (pair.first.id() != pair.second.id()) {
            access_column(pair.second).impl().quadratic().remove({ t_ctr, pair.first });
        }
    }

    auto& rhs = access_rhs();
    rhs.remove(t_ctr);
}

void idol::Matrix::remove_column_from_rows(const Var &t_var) {
    auto& column = access_column(t_var);
    for (const auto& [ctr, ptr_to_coeff] : column.impl().linear().refs()) {
        access_row(ctr).linear().remove(t_var);
    }
    for (const auto& [pair, ptr_to_coeff] : column.impl().quadratic().refs()) {
        access_row(pair.first).quadratic().remove({ t_var, pair.second });
    }

    auto& obj = access_obj();
    obj.linear().remove(t_var);
}

void idol::Matrix::replace_objective(Expr<Var> &&t_objective) {
    auto& obj = access_obj();

    for (const auto& [var, coeff] : obj.linear()) {
        access_column(var).impl().refs().reset_constant();
    }

    obj = std::move(t_objective);

    for (auto [var, ref] : obj.linear().refs()) {
        access_column(var).impl().refs().set_constant(std::move(ref));
    }

}

void idol::Matrix::replace_right_handside(LinExpr<Ctr> &&t_right_handside) {
    auto& rhs = access_rhs();

    for (const auto& [ctr, coeff] : rhs) {
        access_row(ctr).impl().refs().reset_constant();
    }

    rhs = std::move(t_right_handside);

    for (auto [ctr, ref] : rhs.refs()) {
        access_row(ctr).impl().refs().set_constant(std::move(ref));
    }
}

void idol::Matrix::add_to_obj(const Var& t_var, double t_constant) {

    auto& column = access_column(t_var);

    if (std::abs(column.obj()) < Tolerance::Sparsity) {
        column.set_obj(t_constant);
        access_obj().linear().refs().set(t_var, column.impl().refs().get_constant());
        return;
    }

    if (std::abs(t_constant) < Tolerance::Sparsity) {
        access_obj().linear().remove(t_var);
    }
    column.set_obj(t_constant);

}

void idol::Matrix::add_to_rhs(const Ctr &t_ctr, double t_constant) {

    auto& row = access_row(t_ctr);

    if (std::abs(row.rhs()) < Tolerance::Sparsity && std::abs(t_constant) >= Tolerance::Sparsity) {
        row.set_rhs(t_constant);
        access_rhs().refs().set(t_ctr, row.impl().refs().get_constant());
        return;
    }

    if (std::abs(t_constant) < Tolerance::Sparsity) {
        access_rhs().remove(t_ctr);
    }

    row.set_rhs(t_constant);

}

void idol::Matrix::update_matrix_coefficient(const Ctr &t_ctr, const Var &t_var, double t_constant) {

    if (std::abs(t_constant) < Tolerance::Sparsity) {
        access_column(t_var).impl().linear().remove(t_ctr);
        access_row(t_ctr).linear().remove(t_var);
        return;
    }

    auto& column = access_column(t_var);

    auto it = column.impl().linear().refs().find(t_ctr);

    if (it == column.impl().linear().refs().end()) {
        auto inserted = column.impl().linear().refs().emplace(t_ctr, MatrixCoefficient(t_constant));
        access_row(t_ctr).linear().refs().emplace(t_var, (*inserted).second);
    } else {
        (*it).second.value() = t_constant;
    }

}
