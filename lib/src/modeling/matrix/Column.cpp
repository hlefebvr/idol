//
// Created by henri on 04/10/22.
//
#include "../../../include/modeling/matrix/Column.h"
#include "../../../include/modeling/expressions/operations/operators_Ctr.h"

const Column Column::EmptyColumn;

Column Column::fix(const Solution::Primal &t_primals) const {
    Column result;

    for (const auto& [ctr, constant] : linear()) {
        result.linear() += constant.fix(t_primals) * ctr;
    }

    result.obj() = obj().fix(t_primals);

    return result;
}
