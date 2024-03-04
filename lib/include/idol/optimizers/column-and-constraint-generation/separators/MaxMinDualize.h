//
// Created by henri on 08.02.24.
//

#ifndef IDOL_MAXMINDUALIZE_H
#define IDOL_MAXMINDUALIZE_H

#include "idol/optimizers/column-and-constraint-generation/ColumnAndConstraintGenerationSeparator.h"
#include "idol/optimizers/OptimizerFactory.h"

namespace idol::ColumnAndConstraintGenerationSeparators {
    class MaxMinDualize;
}

class idol::ColumnAndConstraintGenerationSeparators::MaxMinDualize : public idol::ColumnAndConstraintGenerationSeparator {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    Model create_second_stage_primal_problem(Env& t_env, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) const;
    void make_feasibility_problem(Model& t_model) const;
    Solution::Primal solve_max_min(const Model& t_max, const Model& t_min) const;
    void add_lower_level_constraint(Model& t_primal, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution, const Ctr& t_ctr) const;
    void set_lower_level_objective(Model& t_primal, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) const;

    [[nodiscard]] static Expr<Var, Var> fix(const LinExpr<Var>& t_expr, const Optimizers::ColumnAndConstraintGeneration &t_parent, const Solution::Primal &t_upper_level_solution) ;
public:
    MaxMinDualize() = default;

    MaxMinDualize(const MaxMinDualize& t_src);

    [[nodiscard]] ColumnAndConstraintGenerationSeparator *clone() const override;

    Solution::Primal operator()(const Optimizers::ColumnAndConstraintGeneration &t_parent,
                                const Solution::Primal &t_upper_level_solution,
                                const Row& t_row,
                                CtrType t_type) const override;

    MaxMinDualize &with_optimizer(const OptimizerFactory& t_optimizer_factory);
};

#endif //IDOL_MAXMINDUALIZE_H
