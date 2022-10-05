//
// Created by henri on 05/10/22.
//
#include "algorithms/solution-strategies/cut-generation/generators/LazyCuts.h"
#include "modeling/expressions/Row.h"
#include "modeling/solutions/Solution.h"

LazyCuts::LazyCuts(const Model &t_rmp_model, const Model &t_sp_model)
    : BaseGenerator<Var>(t_rmp_model, t_sp_model) {

}

Row LazyCuts::get_separation_objective(const Solution::Primal &t_primals) {

    double rhs_factor = 1.;

    Row result;

    if (type() == LessOrEqual) {
        result = Row(constant(), 0.);
        rhs_factor = -1;
    } else {
        result = Row(-1. * constant(), 0.);
    }

    for (auto [var, row] : *this) {
        row *= rhs_factor * t_primals.get(var);
        result += row;
    }
    return result;
}

TempCtr LazyCuts::create_cut(const Solution::Primal &t_primals) const {
    Row row;

    double rhs = 0.;
    for (const auto& [var, coeff] : constant()) {
        rhs += coeff.numerical() * t_primals.get(var);
    }
    row.set_rhs(rhs);

    for (const auto& [ctr, expr] : *this) {
        double coeff = expr.rhs().numerical();
        for (const auto& [var, value] : expr.lhs()) {
            coeff += value.numerical() * t_primals.get(var);
        }
        row.lhs().set(ctr, coeff);
    }

    return { std::move(row), GreaterOrEqual };
}

