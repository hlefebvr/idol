//
// Created by henri on 04/10/22.
//
#include <cassert>
#include "../../../include/modeling/matrix/Matrix.h"
#include "../../../include/modeling/variables/Var.h"
#include "../../../include/modeling/expressions/Column.h"
#include "../../../include/modeling/expressions/Row.h"
#include "../../../include/modeling/expressions/Expr.h"
#include "../../../include/modeling/environment/ObjectManager.h"
#include "../../../include/modeling/constraints/impl_Constraint.h"

void Matrix::apply_on_column(const Var &t_var, const std::function<void(const Ctr&, MatrixCoefficientReference&&)>& t_function) {
    for (const auto& [ctr, ptr_to_coeff] : t_var.column().components().linear().m_map) {
        t_function(ctr, MatrixCoefficientReference(*ptr_to_coeff));
    }
}

void Matrix::apply_on_row(const Ctr &t_ctr, const std::function<void(const Var &, MatrixCoefficientReference&&)> &t_function) {
    for (const auto& [var, ptr_to_coeff] : t_ctr.row().lhs().m_map) {
        t_function(var, MatrixCoefficientReference(*ptr_to_coeff));
    }
}

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

void Matrix::add_to_obj(Expr<Var> &t_objective, const Var &t_var, Column &t_column) {
    if (!t_column.objective_coefficient().is_zero()) {
        t_objective.linear().set(t_var, MatrixCoefficientReference(*t_column.m_components.m_constant));
    }
}

void Matrix::apply_obj(const Expr<Var> &t_objective,
                       const std::function<void(const Var &, MatrixCoefficientReference &&)> &t_function) {

    for (const auto& [var, ptr_to_coeff] : t_objective.linear().m_map) {
        t_function(var, MatrixCoefficientReference(*ptr_to_coeff));
    }

}
