//
// Created by henri on 15/09/22.
//
#include "algorithms/branch-and-cut-and-price/ColumnGenerator.h"

void ColumnGenerator::set(const Ctr &t_ctr, Expr t_expr, double t_offset) {
    auto it = m_values.find(t_ctr);
    if (it == m_values.end()) {
        m_values.emplace(t_ctr, Row(std::move(t_expr), t_offset));
    } else {
        it->second = Row(std::move(t_expr), t_offset);
    }
}

const Row &ColumnGenerator::get(const Ctr &t_ctr) const {
    auto it = m_values.find(t_ctr);
    return it == m_values.end() ? Row::EmptyRow : it->second;
}

