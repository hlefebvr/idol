//
// Created by henri on 08.02.24.
//

#ifndef IDOL_BILEVEL_H
#define IDOL_BILEVEL_H

#include "CCGSeparator.h"
#include "idol/modeling/bilevel-optimization/LowerLevelDescription.h"

namespace idol::Optimizers::Robust {
    class ColumnAndConstraintGeneration;
}

namespace idol::Robust::CCGSeparators {
    class Bilevel;
}

class idol::Robust::CCGSeparators::Bilevel : public idol::Robust::CCGSeparator {
protected:
    void add_lower_level_variables(Model& t_hpr, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent) const;
    void add_lower_level_constraints(Model& t_hpr, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) const;
    void add_lower_level_constraint(Model& t_hpr, const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution, const Ctr& t_ctr) const;
    idol::Expr<idol::Var, idol::Var> set_upper_and_lower_objectives(idol::Model &t_hpr, const idol::Optimizers::Robust::ColumnAndConstraintGeneration &t_parent, const idol::Solution::Primal &t_upper_level_solution, const idol::Row &t_row, idol::CtrType t_type) const;

    static Solution::Primal solve_bilevel(const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                   idol::Model& t_model,
                                   const idol::Bilevel::LowerLevelDescription& t_description) ;
public:
    CCGSeparator *clone() const override;

    Solution::Primal solve_feasibility_separation_problem(const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                                          const Solution::Primal &t_upper_level_solution) const override;

    Solution::Primal solve_separation_problem(const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                const Solution::Primal &t_upper_level_solution,
                                const Row& t_row,
                                CtrType t_type) const override;
};

#endif //IDOL_BILEVEL_H
