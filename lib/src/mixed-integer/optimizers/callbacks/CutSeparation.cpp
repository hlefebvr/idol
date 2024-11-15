//
// Created by henri on 11/04/23.
//

#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CutSeparation.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::impl::CutSeparation::CutSeparation(CallbackEvent t_triggering_event,
                                   Model *t_separation_problem,
                                   TempCtr t_cut)
    : m_triggering_event(t_triggering_event),
      m_separation_problem(t_separation_problem),
      m_cut(std::move(t_cut)) {

    if (m_cut.type() == Equal) {
        throw Exception("Separating equality constraints is not available.");
    }

}

void idol::impl::CutSeparation::operator()(CallbackEvent t_event) {

    throw Exception("TODO: Was using Constant");

    /*

    if (t_event != m_triggering_event) {
        return;
    }

    const auto& current_solution = primal_solution();

    auto [objective, sense] = create_separation_objective(current_solution);

    m_separation_problem->set_obj_expr(std::move(objective));
    m_separation_problem->set_obj_sense(sense);

    m_separation_problem->optimize();

    const auto status = m_separation_problem->get_status();

    if (status != Optimal && status != Feasible) {
        return;
    }

    const unsigned int n_solutions = m_separation_problem->get_n_solutions();

    unsigned int k = 0;
    for ( ; k < n_solutions ; ++k) {

        m_separation_problem->set_solution_index(k);

        if (k == 0 && m_separation_problem->get_best_obj() >= -m_tolerance) {
            break;
        }

        const auto& solution = save_primal(*m_separation_problem);

        TempCtr cut(m_cut.row().fix(solution), m_cut.type());

        hook_add_cut(cut);
    }

     */
}

std::pair<idol::Expr<idol::Var>, idol::ObjectiveSense>
idol::impl::CutSeparation::create_separation_objective(const idol::PrimalPoint &t_primal_solution) {

    throw Exception("TODO: Was using Constant");
    /*
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
     */
}
