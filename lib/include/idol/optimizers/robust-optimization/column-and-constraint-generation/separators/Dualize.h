//
// Created by henri on 08.02.24.
//

#ifndef IDOL_MAXMINDUALIZE_H
#define IDOL_MAXMINDUALIZE_H

#include "CCGSeparator.h"
#include "idol/optimizers/OptimizerFactory.h"

namespace idol::Robust::CCGSeparators {
    class Dualize;
}

class idol::Robust::CCGSeparators::Dualize : public idol::Robust::CCGSeparator {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    Model create_second_stage_primal_problem(Env& t_env,
                                             const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                             const Solution::Primal &t_upper_level_solution) const;

    void make_feasibility_problem(Model& t_model) const;

    Solution::Primal solve_max_min(const Model& t_max, const Model& t_min, double t_time_limit) const;

    void add_lower_level_constraint(Model& t_primal,
                                    const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                    const Solution::Primal &t_upper_level_solution,
                                    const Ctr& t_ctr) const;

    void set_second_stage_primal_objective(Model& t_primal,
                                           const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                           const Solution::Primal &t_upper_level_solution,
                                           const Row& t_row, CtrType t_type) const;

    [[nodiscard]] static Expr<Var, Var> fix(const LinExpr<Var>& t_expr,
                                            const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                            const Solution::Primal &t_upper_level_solution) ;
public:
    Dualize() = default;

    Dualize(const Dualize& t_src);

    [[nodiscard]] CCGSeparator *clone() const override;

    Solution::Primal operator()(const Optimizers::Robust::ColumnAndConstraintGeneration &t_parent,
                                const Solution::Primal &t_upper_level_solution,
                                const Row& t_row,
                                CtrType t_type) const override;

    Dualize &with_optimizer(const OptimizerFactory& t_optimizer_factory);
};

#endif //IDOL_DUALIZE_H
