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

    if (!m_cut.row().quadratic().empty()) {
        throw Exception("Adding non-linear cut is not available.");
    }

    if (m_cut.type() == Equal) {
        throw Exception("Separating equality constraints is not available.");
    }

}

void idol::impl::CutSeparation::operator()(CallbackEvent t_event) {

    if (t_event != m_triggering_event) {
        return;
    }

    const auto& current_solution = primal_solution();

    auto [objective, sense] = create_separation_objective(current_solution);

    m_separation_problem->set_obj_expr(std::move(objective));
    m_separation_problem->set_obj_sense(sense);

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

        idol_Log(Debug, "Solution " << k << " has a violation of " << std::abs(m_separation_problem->get_best_obj()) << ".")

        if (k == 0 && m_separation_problem->get_best_obj() >= -m_tolerance) {
            break;
        }

        const auto& solution = save_primal(*m_separation_problem);

        TempCtr cut(m_cut.row().fix(solution), m_cut.type());

        idol_Log(Trace, "The following cut was added: " << cut << ".")

        hook_add_cut(cut);
    }

    idol_Log(Info, "In total, " << k << " cuts have been added.")

}

std::pair<idol::Expr<idol::Var, idol::Var>, idol::ObjectiveSense>
idol::impl::CutSeparation::create_separation_objective(const idol::Solution::Primal &t_primal_solution) {

    const auto sense = m_cut.type() == LessOrEqual ? Minimize : Maximize;

    const auto& row = m_cut.row();

    ::idol::Expr result = row.rhs().numerical();

    for (const auto& [param, coeff] : row.rhs().linear()) {
        result += coeff * param.as<Var>();
    }

    for (const auto& [var, constant] : row.linear()) {
        ::idol::Expr term = -constant.numerical();
        for (const auto& [param, coeff] : constant.linear()) {
            term += -coeff * param.as<Var>();
        }
        result += term * t_primal_solution.get(var);
    }

    return std::make_pair(std::move(result), sense);
}
