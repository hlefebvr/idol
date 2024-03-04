//
// Created by henri on 16.10.23.
//
#include "idol/optimizers/mixed-integer-programming/callbacks/heuristics/RENS.h"
#include "idol/modeling//models/Model.h"

idol::Heuristics::RENS::RENS(const RENS& t_src)
    : m_optimizer_factory(t_src.m_optimizer_factory->clone()),
      m_minimum_ratio_of_variables_to_fix(t_src.m_minimum_ratio_of_variables_to_fix),
      m_minimum_ratio_of_integer_variables_to_fix(t_src.m_minimum_ratio_of_integer_variables_to_fix)
{

}

idol::Heuristics::RENS &idol::Heuristics::RENS::with_optimizer(const idol::OptimizerFactory& t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("An optimizer for RENS has already been given.");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::Heuristics::RENS::Strategy::Strategy(double t_minimum_ratio_of_integer_variables_to_fix,
                                           double t_minimum_ratio_of_variables_to_fix,
                                           OptimizerFactory* t_optimizer_factory)
     : m_minimum_ratio_of_integer_variables_to_fix(t_minimum_ratio_of_integer_variables_to_fix),
       m_minimum_ratio_of_variables_to_fix(t_minimum_ratio_of_variables_to_fix),
       m_optimizer_factory(t_optimizer_factory) {

}

void idol::Heuristics::RENS::Strategy::operator()(idol::CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    const auto& primal_solution = this->primal_solution();

    std::unique_ptr<Model> model(original_model().clone());

    model->unuse();

    unsigned int n_fixed_variables = 0;
    unsigned int n_integer_variables = 0;
    for (const auto& var : model->vars()) {

        if (model->get_var_type(var) == Continuous) {
            continue;
        }

        ++n_integer_variables;

        const double value = primal_solution.get(var);

        if (is_integer(value, Tolerance::Integer)) {
            ++n_fixed_variables;
        }

        model->set_var_lb(var, std::floor(value));
        model->set_var_ub(var, std::ceil(value));

    }

    if (n_fixed_variables < m_minimum_ratio_of_integer_variables_to_fix * n_integer_variables
            || n_fixed_variables < m_minimum_ratio_of_variables_to_fix * model->vars().size() ) {
        return;
    }

    ++m_n_relevant_calls;

    if (m_n_relevant_calls % m_frequency != 0) {
        return;
    }

    model->use(*m_optimizer_factory);

    model->optimize();

    if (const auto status = model->get_status() ; status != Optimal && status != Feasible) {
        return;
    }

    submit_heuristic_solution(save_primal(*model));

}

