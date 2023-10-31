//
// Created by henri on 30/03/23.
//
#include "idol/optimizers/archive/column-generation/IntegerMaster.h"
#include "idol/optimizers/archive/column-generation/ArchivedColumnGeneration.h"

idol::Heuristics::IntegerMaster::IntegerMaster(const IntegerMaster& t_src)
    : m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr),
      m_integer_columns(t_src.m_integer_columns),
      m_time_limit(t_src.m_time_limit),
      m_iteration_limit(t_src.m_time_limit),
      m_max_depth(t_src.m_max_depth),
      m_frequency(t_src.m_frequency)
    {}

idol::BranchAndBoundCallback<idol::NodeVarInfo> *idol::Heuristics::IntegerMaster::operator()() {

    if (!m_optimizer_factory) {
        throw Exception("No solver was given to solve the integer master problem, please call IntegerMaster.rst::with_optimizer to configure.");
    }

    auto* result = new Strategy(*m_optimizer_factory);

    if (m_integer_columns.has_value()) {
        result->set_integer_columns(m_integer_columns.value());
    }

    if (m_time_limit.has_value()) {
        result->set_time_limit(m_time_limit.value());
    }

    if (m_iteration_limit.has_value()) {
        result->set_iteration_limit(m_iteration_limit.value());
    }

    if (m_max_depth.has_value()){
        result->set_max_depth(m_max_depth.value());
    }

    if (m_frequency.has_value()) {
        result->set_frequency(m_frequency.value());
    }

    return result;
}

idol::Heuristics::IntegerMaster &idol::Heuristics::IntegerMaster::with_optimizer(const OptimizerFactory &t_optimizer) {

    if (m_optimizer_factory) {
        throw Exception("A solver has already been given.");
    }

    m_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}

idol::BranchAndBoundCallbackFactory<idol::NodeVarInfo> *idol::Heuristics::IntegerMaster::clone() const {
    return new IntegerMaster(*this);
}

idol::Heuristics::IntegerMaster &idol::Heuristics::IntegerMaster::with_integer_columns(bool t_value) {
    m_integer_columns = t_value;
    return *this;
}

idol::Heuristics::IntegerMaster &idol::Heuristics::IntegerMaster::with_time_limit(double t_time_limit) {
    m_time_limit = t_time_limit;
    return *this;
}

idol::Heuristics::IntegerMaster &idol::Heuristics::IntegerMaster::with_iteration_limit(unsigned int t_iteration_limit) {
    m_iteration_limit = t_iteration_limit;
    return *this;
}

idol::Heuristics::IntegerMaster &idol::Heuristics::IntegerMaster::with_max_depth(unsigned int t_max_depth) {
    m_max_depth = t_max_depth;
    return *this;
}

idol::Heuristics::IntegerMaster &idol::Heuristics::IntegerMaster::with_frequency(unsigned int t_frequency) {
    m_frequency = t_frequency;
    return *this;
}

idol::Heuristics::IntegerMaster::Strategy::Strategy(const OptimizerFactory &t_optimizer)
        : m_optimizer_factory(t_optimizer.clone()) {

}

void idol::Heuristics::IntegerMaster::Strategy::operator()(CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    if (m_max_depth < this->node().level()) {
        return;
    }

    unsigned int n_relevant_calls = m_n_relevant_calls++;

    if ( n_relevant_calls % m_frequency != 0) {
        return;
    }

    const auto& relaxation = this->relaxation();
    const auto& column_generation_optimizer = relaxation.optimizer().as<Optimizers::ArchivedColumnGeneration>();
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

    integer_master->optimizer().set_param_time_limit(std::min(m_time_limit, relaxation.optimizer().get_remaining_time()));
    // TODO set bound stop
    integer_master->optimizer().set_param_iteration_limit(m_iteration_limit);

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

    auto* info = new NodeVarInfo();
    info->set_primal_solution(std::move(solution));

    submit_heuristic_solution(info);

}
