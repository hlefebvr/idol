//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATOR_H
#define OPTIMIZE_COLUMNGENERATOR_H

#include "modeling/numericals.h"
#include "modeling/Types.h"
#include "modeling/variables/TempVar.h"

namespace Solution {
    class Primal;
    class Dual;
}

class ColumnGenerationSP;

class ColumnGenerator {
    VarType m_type = Continuous;
    double m_lower_bound = 0.;
    double m_upper_bound = Inf;
public:
    ColumnGenerator() = default;

    virtual ~ColumnGenerator() = default;

    [[nodiscard]] VarType type() const { return m_type; }

    [[nodiscard]] double lb() const { return m_lower_bound; }

    [[nodiscard]] double ub() const { return m_upper_bound; }

    [[nodiscard]] virtual TempVar create_column(const Solution::Primal& t_primal_solution) const = 0;

    [[nodiscard]] virtual Row get_pricing_objective(const Solution::Dual& t_dual_solution) = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution(const ColumnGenerationSP& t_subproblem, const Solution::Primal& t_rmp_primals) const = 0;

    virtual bool set_lower_bound(const Var& t_var, double t_lb, ColumnGenerationSP& t_subproblem) = 0;

    virtual bool set_upper_bound(const Var& t_var, double t_lb, ColumnGenerationSP& t_subproblem) = 0;

    virtual std::optional<Ctr> contribute_to_add_constraint(TempCtr& t_temporary_constraint, ColumnGenerationSP& t_subproblem) = 0;

    virtual bool update_constraint_rhs(const Ctr& t_ctr, double t_rhs, ColumnGenerationSP& t_subproblem) = 0;

    virtual bool remove_constraint(const Ctr& t_ctr, ColumnGenerationSP& t_subproblem) = 0;
};

#endif //OPTIMIZE_COLUMNGENERATOR_H