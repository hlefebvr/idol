//
// Created by henri on 30/03/23.
//
#include "optimizers/column-generation/IntegerMasterHeuristic.h"
#include "optimizers/column-generation/ColumnGeneration.h"

idol::IntegerMasterHeuristic::IntegerMasterHeuristic(const IntegerMasterHeuristic& t_src)
    :m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr) {}

idol::BranchAndBoundCallback<idol::NodeInfo> *idol::IntegerMasterHeuristic::operator()() {

    if (!m_optimizer_factory) {
        throw Exception("No solver was given to solve the integer master problem, please call IntegerMasterHeuristic::with_optimizer to configure.");
    }

    auto* result = new Strategy(*m_optimizer_factory);

    if (m_integer_columns.has_value()) {
        result->set_integer_columns(m_integer_columns.value());
    }

    return result;
}

idol::IntegerMasterHeuristic &idol::IntegerMasterHeuristic::with_optimizer(const OptimizerFactory &t_optimizer) {

    if (m_optimizer_factory) {
        throw Exception("A solver has already been given.");
    }

    m_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}

idol::BranchAndBoundCallbackFactory<idol::NodeInfo> *idol::IntegerMasterHeuristic::clone() const {
    return new IntegerMasterHeuristic(*this);
}

idol::IntegerMasterHeuristic &idol::IntegerMasterHeuristic::with_integer_columns(bool t_value) {
    m_integer_columns = t_value;
    return *this;
}

idol::IntegerMasterHeuristic::Strategy::Strategy(const OptimizerFactory &t_optimizer)
        : m_optimizer_factory(t_optimizer.clone()) {

}

void idol::IntegerMasterHeuristic::Strategy::operator()(CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    const auto& relaxation = this->relaxation();
    const auto& column_generation_optimizer = relaxation.optimizer().as<Optimizers::ColumnGeneration>();
    const auto& original_model = this->original_model();

    std::unique_ptr<Model> integer_master(column_generation_optimizer.master().clone());

    integer_master->unuse();

    for (const auto& var : original_model.vars()) {
        const VarType type = original_model.get_var_type(var);
        if (integer_master->has(var)) {
            integer_master->set_var_type(var, type);
        }
    }

    if (m_integer_columns) {
        for (const auto &subproblem: column_generation_optimizer.subproblems()) {
            for (const auto &[alpha, generator]: subproblem.present_generators()) {
                integer_master->set_var_type(alpha, Binary);
            }
        }
    }

    integer_master->use(*m_optimizer_factory);

    integer_master->optimizer().set_param_time_limit(relaxation.optimizer().get_remaining_time());

    integer_master->optimize();

    const int status = integer_master->get_status();

    if (status != Optimal && status != Feasible) {
        return;
    }

    auto solution = save_primal(*integer_master);

    // search for alpha = 1, add generator to solution
    for (const auto& subproblem : column_generation_optimizer.subproblems()) {
        for (const auto &[alpha, generator]: subproblem.present_generators()) {
            if (solution.get(alpha) > .5) {
                solution.merge_without_conflict(generator);
                solution.set(alpha, 0.);
            }
        }
    }

    auto* info = new NodeInfo();
    info->set_primal_solution(std::move(solution));

    submit_heuristic_solution(info);

}
