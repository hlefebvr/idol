//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_DEPRECATED_COLUMN_H
#define OPTIMIZE_DEPRECATED_COLUMN_H

#include "modeling/constraints/Constraint.h"
#include "Deprecated_ColumnOrRow.h"

class Deprecated_Column : public Deprecated_ColumnOrRow<Ctr> {
public:
    Deprecated_Column() = default;
    explicit Deprecated_Column(Constant t_objective_coefficient) : Deprecated_ColumnOrRow<Ctr>(std::move(t_objective_coefficient)) {}

    Deprecated_Column(const Deprecated_Column& t_src) = default;
    Deprecated_Column(Deprecated_Column&& t_src) noexcept = default;

    Deprecated_Column& operator=(const Deprecated_Column& t_src) = default;
    Deprecated_Column& operator=(Deprecated_Column&& t_src) noexcept = default;
};

#endif //OPTIMIZE_DEPRECATED_COLUMN_H
