//
// Created by henri on 11/04/23.
//

#include "optimizers/callbacks/CutSeparation.h"
#include "modeling/expressions/operations/operators.h"

impl::CutSeparation::CutSeparation(Model *t_separation_problem, TempCtr t_cut)
    : m_separation_problem(t_separation_problem),
      m_cut(std::move(t_cut)) {

}

void impl::CutSeparation::operator()(BranchAndBoundEvent t_event) {

    if (t_event != IncumbentSolution) {
        return;
    }

    const auto& current_solution = primal_solution();

    const auto& row = m_cut.row();

    if (!row.quadratic().empty()) {
        throw Exception("Adding non-linear cut is not available in Gurobi.");
    }

    ::Expr objective = row.rhs().numerical();

    for (const auto& [param, coeff] : row.rhs()) {
        objective += coeff * param.as<Var>();
    }

    for (const auto& [var, constant] : row.linear()) {
        ::Expr term = -constant.numerical();
        for (const auto& [param, coeff] : constant) {
            term += -coeff * param.as<Var>();
        }
        objective += term * current_solution.get(var);
    }

    m_separation_problem->set_obj_expr(std::move(objective));

    m_separation_problem->optimize();

    const auto status = m_separation_problem->get_status();

    if (status != Optimal && status != Feasible) {
        return;
    }

    const auto& solution = save_primal(*m_separation_problem);

    if (solution.objective_value() > 0) {
        return;
    }

    TempCtr cut(m_cut.row().fix(solution), m_cut.type());

    hook_add_cut(cut);

}
