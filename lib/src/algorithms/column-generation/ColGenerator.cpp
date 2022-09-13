//
// Created by henri on 12/09/22.
//
#include "algorithms/column-generation/ColGenerator.h"

void ColGenerator::set(const Ctr &t_ctr, Row t_row) {
    auto it = m_column.find(t_ctr);
    if (it == m_column.end()) {
        m_column.emplace(t_ctr, std::move(t_row));
    } else {
        it->second = std::move(t_row);
    }
}

const Row &ColGenerator::get(const Ctr &t_ctr) const {
    auto it = m_column.find(t_ctr);
    return it == m_column.end() ? Row::EmptyRow : it->second;
}

void ColGenerator::set(const Ctr &t_ctr, Expr t_expr, double t_constant) {
    set(t_ctr, Row(std::move(t_expr), t_constant));
}

void ColGenerator::set_objective(Row t_row) {
    m_objective = std::move(t_row);
}

void ColGenerator::set_objective(Expr t_expr, double t_constant) {
    set_objective(Row(std::move(t_expr), t_constant));
}
