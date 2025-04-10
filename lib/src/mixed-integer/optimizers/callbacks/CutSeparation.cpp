//
// Created by henri on 11/04/23.
//

#include "idol/mixed-integer/optimizers/callbacks/cutting-planes/CutSeparation.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"

idol::impl::CutSeparation::CutSeparation(CallbackEvent t_triggering_event,
                                   Model *t_separation_problem,
                                   GenerationPattern<Ctr> t_cut,
                                   CtrType t_type)
    : m_triggering_event(t_triggering_event),
      m_separation_problem(t_separation_problem),
      m_cut_pattern(std::move(t_cut)),
      m_separation_objective_pattern(!m_cut_pattern),
      m_type(t_type) {

    if (m_type == Equal) {
        throw Exception("Separating equality constraints is not available.");
    }

    if (m_type == LessOrEqual) {
        m_separation_objective_pattern *= -1;
    }

}

void idol::impl::CutSeparation::operator()(CallbackEvent t_event) {

    if (t_event != m_triggering_event) {
        return;
    }

    const auto& current_solution = primal_solution();

    auto objective = m_separation_objective_pattern(current_solution);

    m_separation_problem->set_obj_expr(std::move(objective));
    m_separation_problem->set_obj_sense(Minimize);

    m_separation_problem->optimizer().set_param_time_limit(this->original_model().optimizer().get_remaining_time());
    m_separation_problem->optimize();

    const auto status = m_separation_problem->get_status();


    if (status != Optimal && status != Feasible) {
        return;
    }

    const unsigned int n_solutions = m_separation_problem->get_n_solutions();

    for (unsigned int k = 0 ; k < n_solutions ; ++k) {

        m_separation_problem->set_solution_index(k);

        const double objective_value = m_separation_problem->get_best_obj();

        if (objective_value >= -m_tolerance) {
            break;
        }

        const auto& solution = save_primal(*m_separation_problem);
        auto expr = m_cut_pattern(solution);

        TempCtr cut(std::move(expr.linear()), m_type, -expr.constant());

        hook_add_cut(cut);
    }

}
