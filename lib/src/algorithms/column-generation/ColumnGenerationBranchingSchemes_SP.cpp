//
// Created by henri on 15/09/22.
//
#include "../../../include/algorithms/column-generation/ColumnGenerationBranchingSchemes_SP.h"
#include "../../../include/algorithms/column-generation/ColumnGenerationSP.h"
#include "modeling/variables/Attributes_Var.h"

void ColumnGenerationBranchingSchemes::SP::set_lower_bound(const Var &t_var, double t_lb, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_columns_violating_lower_bound(t_var, t_lb);
    t_subproblem.exact_solution_strategy().set(Attr::Var::Lb, t_var, t_lb);

}

void ColumnGenerationBranchingSchemes::SP::set_upper_bound(const Var &t_var, double t_ub, ColumnGenerationSP &t_subproblem) {

    t_subproblem.remove_columns_violating_upper_bound(t_var, t_ub);
    t_subproblem.exact_solution_strategy().set(Attr::Var::Ub, t_var, t_ub);

}

std::optional<Ctr> ColumnGenerationBranchingSchemes::SP::contribute_to_add_constraint(TempCtr &t_temporary_constraint, ColumnGenerationSP& t_subproblem) {

    for (const auto& [var, constant] : t_temporary_constraint.row().linear()) {
        if (!t_subproblem.exact_solution_strategy().get(Attr::Var::Status, var)) {
            return {};
        }
    }

    t_subproblem.remove_columns_violating_constraint(t_temporary_constraint);

    auto result = t_subproblem.exact_solution_strategy().add_ctr(std::move(t_temporary_constraint));

    idol_Log(Trace, ColumnGenerationBranchingSchemes::SP, "Constraint " << result << " was added to subproblem.")

    return { result };
}
