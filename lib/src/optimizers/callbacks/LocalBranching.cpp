//
// Created by henri on 16.10.23.
//
#include "idol/optimizers/callbacks/LocalBranching.h"
#include "idol/modeling//models/Model.h"
#include "idol/modeling//expressions/operations/operators.h"

idol::Heuristics::LocalBranching::LocalBranching(const LocalBranching& t_src)
    : m_optimizer_factory(t_src.m_optimizer_factory->clone()),
      m_initial_neighbourhood_size(t_src.m_initial_neighbourhood_size)
{

}

idol::Heuristics::LocalBranching &idol::Heuristics::LocalBranching::with_optimizer(const idol::OptimizerFactory& t_optimizer_factory) {

    if (m_optimizer_factory) {
        throw Exception("An optimizer for LocalBranching has already been given.");
    }

    m_optimizer_factory.reset(t_optimizer_factory.clone());

    return *this;
}

idol::Heuristics::LocalBranching::Strategy::Strategy(unsigned int t_neighbourhood_size,
                                           OptimizerFactory* t_optimizer_factory)
     : m_neighbourhood_size(t_neighbourhood_size),
       m_initial_neighbourhood_size(t_neighbourhood_size),
       m_optimizer_factory(t_optimizer_factory) {

}

void idol::Heuristics::LocalBranching::Strategy::operator()(idol::CallbackEvent t_event) {

    if (t_event != IncumbentSolution || !m_execute) {
        return;
    }

    const auto& primal_solution = this->primal_solution();

    std::cout << primal_solution.objective_value() << std::endl;

    std::unique_ptr<Model> model(original_model().clone());

    model->unuse();

    Expr<Var, Var> distance_to_incumbent;

    for (const auto& var : model->vars()) {

        if (const auto type = model->get_var_type(var) ; type == Continuous) {
            continue;
        } else if (type == Integer) {
            throw Exception("LocalBranching is not implemented for general integers.");
        }

        if (equals(primal_solution.get(var), 0., Tolerance::Integer)) {
            distance_to_incumbent += var;
        } else {
            distance_to_incumbent += 1 - var;
        }

    }

    auto neighbourhood_ctr = model->add_ctr(distance_to_incumbent <= m_neighbourhood_size);
    auto objective_ctr = model->add_ctr(model->get_obj_expr() <= .9 * primal_solution.objective_value());

    model->use(*m_optimizer_factory);

    model->optimize();

    if (const auto status = model->get_status() ; status == Optimal || status == Feasible) {
        submit_heuristic_solution(save_primal(*model));
        return;
    }

    if (const auto reason = model->get_reason() ; reason == TimeLimit || reason == IterLimit) {

        if (m_neighbourhood_size == m_initial_neighbourhood_size) {
            m_neighbourhood_size = m_initial_neighbourhood_size - std::floor(m_initial_neighbourhood_size / 2);
        } else {
            m_execute = false;
        }

    } else if (reason == Proved || reason == NotSpecified) {

        if (m_neighbourhood_size == m_initial_neighbourhood_size) {
            m_neighbourhood_size = m_initial_neighbourhood_size + std::ceil(m_initial_neighbourhood_size / 2);
        } else {
            m_execute = false;
        }

    } else {
        std::cout << "The sub-MIP returned with status " << model->get_status() << ", reason " << reason << std::endl;
        throw Exception("Unexpected sub-MIP returned status.");
    }

}

