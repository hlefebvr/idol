//
// Created by henri on 15/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATORS_BASIC_H
#define OPTIMIZE_COLUMNGENERATORS_BASIC_H

#include "algorithms/decomposition/Generators_Basic.h"
#include "modeling/constraints/Constraint.h"
#include "ColumnGenerator.h"
#include "modeling/solutions/Solution.h"

namespace ColumnGenerators {
    class Basic;
}

class ColumnGenerators::Basic : public ColumnGenerator, public Generators::Basic<Ctr> {
protected:
    static void remove_columns_violating_lower_bound(const Var& t_var, double t_lb, ColumnGenerationSP& t_subproblem);

    static void remove_columns_violating_upper_bound(const Var& t_var, double t_ub, ColumnGenerationSP& t_subproblem);

    static void remove_columns_violating_constraint(const TempCtr& t_ctr, ColumnGenerationSP& t_subproblem);

    static void set_lower_bound_sp(const Var& t_var, double t_lb, ColumnGenerationSP& t_subproblem);

    static void set_upper_bound_sp(const Var& t_var, double t_ub, ColumnGenerationSP& t_subproblem);
public:
    Basic(const Model& t_rmp, const Model& t_subproblem);

    Basic(const Basic&) = default;
    Basic(Basic&&) noexcept = default;

    Basic& operator=(const Basic&) = delete;
    Basic& operator=(Basic&&) noexcept = delete;

    TempVar create_column(const Solution::Primal &t_primal_solution) const override;

    Row get_pricing_objective(const Solution::Dual &t_dual_solution) override;

    Solution::Primal primal_solution(const ColumnGenerationSP& t_subproblem, const Solution::Primal& t_rmp_primals) const override;

    bool set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    bool set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    std::optional<Ctr> contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSP& t_subproblem) override;

    bool update_constraint_rhs(const Ctr &t_ctr, double t_rhs, ColumnGenerationSP &t_subproblem) override;

    bool remove_constraint(const Ctr& t_ctr, ColumnGenerationSP& t_subproblem) override;
};

#endif //OPTIMIZE_COLUMNGENERATORS_BASIC_H
