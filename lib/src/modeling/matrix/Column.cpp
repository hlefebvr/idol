//
// Created by henri on 04/10/22.
//
#include "idol/modeling/matrix/Column.h"
#include "idol/modeling/expressions/operations/operators_Ctr.h"

const idol::Column idol::Column::EmptyColumn;

idol::Column idol::Column::fix(const Solution::Primal &t_primals) const {
    Column result;

    for (const auto& [ctr, constant] : linear()) {
        result.linear() += constant.fix(t_primals) * ctr;
    }

    result.obj() = obj().fix(t_primals);

    return result;
}
