//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATOR_H
#define OPTIMIZE_COLUMNGENERATOR_H

#include "algorithms/solution-strategies/decomposition/generators/BaseGenerator.h"
#include "modeling/constraints/Constraint.h"
#include "AbstractColumnGenerator.h"
#include "solvers/solutions/Solution.h"

class ColumnGenerator : public AbstractColumnGenerator, public BaseGenerator<Ctr> {
protected:
    static void remove_columns_violating_lower_bound(const Var& t_var, double t_lb, ColumnGenerationSubProblem& t_subproblem);

    static void remove_columns_violating_upper_bound(const Var& t_var, double t_ub, ColumnGenerationSubProblem& t_subproblem);

    static void remove_columns_violating_constraint(const TempCtr& t_ctr, ColumnGenerationSubProblem& t_subproblem);

    static void set_lower_bound_sp(const Var& t_var, double t_lb, ColumnGenerationSubProblem& t_subproblem);

    void set_upper_bound_sp(const Var& t_var, double t_ub, ColumnGenerationSubProblem& t_subproblem);
public:
    ColumnGenerator(const Model& t_rmp, const Model& t_subproblem);

    ColumnGenerator(const ColumnGenerator&) = default;
    ColumnGenerator(ColumnGenerator&&) noexcept = default;

    ColumnGenerator& operator=(const ColumnGenerator&) = delete;
    ColumnGenerator& operator=(ColumnGenerator&&) noexcept = delete;

    TempVar create_column(const Solution::Primal &t_primal_solution) const override;

    Row get_pricing_objective(const Solution::Dual &t_dual_solution) override;

    Solution::Primal primal_solution(const ColumnGenerationSubProblem& t_subproblem, const Solution::Primal& t_rmp_primals) const override;

    void set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

    void set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSubProblem &t_subproblem) override;

    std::optional<Ctr> contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSubProblem& t_subproblem) override;

    bool update_constraint_rhs(const Ctr &t_ctr, double t_rhs, ColumnGenerationSubProblem &t_subproblem) override;

    bool remove_constraint(const Ctr& t_ctr, ColumnGenerationSubProblem& t_subproblem) override;
};

#endif //OPTIMIZE_COLUMNGENERATOR_H
