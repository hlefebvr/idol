//
// Created by henri on 08.02.24.
//

#ifndef IDOL_MAXMINBILEVEL_H
#define IDOL_MAXMINBILEVEL_H

#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGenerationSeparator.h"

namespace idol::ColumnAndConstraintGenerationSeparators {
    class MaxMinBilevel;
}

class idol::ColumnAndConstraintGenerationSeparators::MaxMinBilevel : public idol::ColumnAndConstraintGenerationSeparator {
protected:
    void add_lower_level_variables(Model& t_hpr, const Optimizers::ColumnAndConstraintGeneration &t_parent) const;
    void add_lower_level_constraints(Model& t_hpr, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) const;
    void add_lower_level_constraint(Model& t_hpr, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution, const Ctr& t_ctr) const;
    idol::Ctr set_upper_and_lower_objectives(idol::Model &t_hpr, const idol::Optimizers::ColumnAndConstraintGeneration &t_parent, const idol::Solution::Primal &t_upper_level_solution, const idol::Row &t_row, idol::CtrType t_type) const;
public:
    ColumnAndConstraintGenerationSeparator *clone() const override;

    Solution::Primal operator()(const Optimizers::ColumnAndConstraintGeneration &t_parent,
                                const Solution::Primal &t_upper_level_solution,
                                const Row& t_row,
                                CtrType t_type) const override;
};

#endif //IDOL_MAXMINBILEVEL_H
