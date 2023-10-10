//
// Created by henri on 11/04/23.
//

#include "optimizers/callbacks/CutSeparation.h"
#include "modeling/expressions/operations/operators.h"

idol::impl::CutSeparation::CutSeparation(CallbackEvent t_triggering_event,
                                   Model *t_separation_problem,
                                   TempCtr t_cut,
                                   LogLevel t_level,
                                   Color t_color)
    : m_triggering_event(t_triggering_event),
      m_separation_problem(t_separation_problem),
      m_cut(std::move(t_cut)),
      m_log_level(t_level),
      m_log_color(t_color) {

}

void idol::impl::CutSeparation::operator()(CallbackEvent t_event) {

    if (t_event != m_triggering_event) {
        return;
    }

    const auto& current_solution = primal_solution();

    const auto& row = m_cut.row();

    if (!row.quadratic().empty()) {
        throw Exception("Adding non-linear cut is not available in Gurobi.");
    }

    ::idol::Expr objective = row.rhs().numerical();

    for (const auto& [param, coeff] : row.rhs().linear()) {
        objective += coeff * param.as<Var>();
    }

    for (const auto& [var, constant] : row.linear()) {
        ::idol::Expr term = -constant.numerical();
        for (const auto& [param, coeff] : constant.linear()) {
            term += -coeff * param.as<Var>();
        }
        objective += term * current_solution.get(var);
    }

    m_separation_problem->set_obj_expr(std::move(objective));

    idol_Log(Debug, "Start solving separation problem");
    m_separation_problem->optimize();
    idol_Log(Debug, "Stop solving separation problem");

    const auto status = m_separation_problem->get_status();

    if (status != Optimal && status != Feasible) {
        return;
    }

    const unsigned int n_solutions = m_separation_problem->get_n_solutions();

    unsigned int k = 0;
    for ( ; k < n_solutions ; ++k) {

        m_separation_problem->set_solution_index(k);

        if (k == 0 && m_separation_problem->get_best_obj() >= -1e-3) {
            std::cout << "STOP" << std::endl;
            break;
        }

        idol_Log(Debug, "Violation: " << m_separation_problem->get_best_obj() << ".")

        const auto& solution = save_primal(*m_separation_problem);

        TempCtr cut(m_cut.row().fix(solution), m_cut.type());

        idol_Log(Trace, "Added " << cut << ".")

        hook_add_cut(cut);
    }

    idol_Log(Info, "Added " << k << " cuts.")

}
