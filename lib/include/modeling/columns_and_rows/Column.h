//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_COLUMN_H
#define OPTIMIZE_COLUMN_H

#include "modeling/constraints/Constraint.h"
#include "ColumnOrRow.h"

class Column : public ColumnOrRow<Ctr> {
public:
    Column() = default;
    explicit Column(Coefficient t_objective_coefficient) : ColumnOrRow<Ctr>(std::move(t_objective_coefficient)) {}

    Column(const Column& t_src) = default;
    Column(Column&& t_src) noexcept = default;

    Column& operator=(const Column& t_src) = default;
    Column& operator=(Column&& t_src) noexcept = default;
};

#endif //OPTIMIZE_COLUMN_H
