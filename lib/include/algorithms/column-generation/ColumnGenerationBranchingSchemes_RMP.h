//
// Created by henri on 19/09/22.
//

#ifndef OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEMES_RMP_H
#define OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEMES_RMP_H

#include "ColumnGenerationBranchingScheme.h"

namespace ColumnGenerationBranchingSchemes {
    class RMP;
}

class ColumnGenerationBranchingSchemes::RMP : public ColumnGenerationBranchingScheme {

    Map<Var, Ctr> m_lower_bound_constraints; // SP Var -> LB Ctr
    Map<Var, Ctr> m_upper_bound_constraints; // SP Var -> UB Ctr

    void set_bound_rmp(const Var& t_subproblem_variable, double t_bound, Map<Var, Ctr>& t_bound_constraints, const std::function<TempCtr(LinExpr<Var>&&, double)>& t_ctr_builder, const std::function<double(const Var&)>& t_get_bound, ColumnGenerationSP& t_subproblem);
public:
    void set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    void set_upper_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) override;

    std::optional<Ctr> contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSP &t_subproblem) override;
};

#endif //OPTIMIZE_COLUMNGENERATIONBRANCHINGSCHEMES_RMP_H
