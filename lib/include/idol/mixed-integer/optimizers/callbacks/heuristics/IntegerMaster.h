//
// Created by henri on 30/03/23.
//

#ifndef IDOL_INTEGERMASTER_H
#define IDOL_INTEGERMASTER_H

#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h"

namespace idol::Heuristics {
    template<class NodeInfoT>  class IntegerMaster;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::Heuristics::IntegerMaster : public BranchAndBoundCallbackFactory<NodeInfoT> {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    std::optional<bool> m_integer_columns;

    std::optional<double> m_time_limit;
    std::optional<unsigned int> m_iteration_limit;
    std::optional<unsigned int> m_max_depth;
    std::optional<unsigned int> m_frequency;

    IntegerMaster(const IntegerMaster& t_src);
public:
    IntegerMaster() = default;

    IntegerMaster(IntegerMaster&&) noexcept = default;

    IntegerMaster& operator=(const IntegerMaster&) = delete;
    IntegerMaster& operator=(IntegerMaster&&) noexcept = default;

    class Strategy : public BranchAndBoundCallback<NodeInfoT> {
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
        bool m_integer_columns = true;
        double m_time_limit = std::numeric_limits<double>::max();
        unsigned int m_iteration_limit = std::numeric_limits<unsigned int>::max();
        unsigned int m_max_depth = 1000;
        unsigned int m_frequency = 1;
        unsigned int m_n_relevant_calls = 0; // Counts the number of calls to the callback which can trigger the heuristic, except for frequency reasons
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        explicit Strategy(const OptimizerFactory& t_optimizer);

        [[nodiscard]] bool with_integer_columns() const { return m_integer_columns; }

        void set_integer_columns(bool t_value) { m_integer_columns = t_value; }

        void set_time_limit(double t_time_limit) { m_time_limit = std::max(0., t_time_limit); }

        void set_iteration_limit(unsigned int t_iteration_limit) { m_iteration_limit = t_iteration_limit; }

        void set_max_depth(unsigned int t_max_depth) { m_max_depth = t_max_depth; }

        void set_frequency(unsigned int t_frequency) { m_frequency = t_frequency; }
    };

    BranchAndBoundCallback<NodeInfoT> *operator()() override;

    [[nodiscard]] BranchAndBoundCallbackFactory<NodeInfoT> *clone() const override;

    IntegerMaster& with_optimizer(const OptimizerFactory& t_optimizer);

    IntegerMaster& with_integer_columns(bool t_value);

    IntegerMaster& with_time_limit(double t_time_limit);

    IntegerMaster& with_iteration_limit(unsigned int t_iteration_limit);

    IntegerMaster& with_max_depth(unsigned int t_max_depth);

    IntegerMaster& with_frequency(unsigned int t_frequency);
};

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT>::IntegerMaster(const IntegerMaster& t_src)
        : m_optimizer_factory(t_src.m_optimizer_factory ? t_src.m_optimizer_factory->clone() : nullptr),
          m_integer_columns(t_src.m_integer_columns),
          m_time_limit(t_src.m_time_limit),
          m_iteration_limit(t_src.m_time_limit),
          m_max_depth(t_src.m_max_depth),
          m_frequency(t_src.m_frequency)
{}

template<class NodeInfoT>
idol::BranchAndBoundCallback<NodeInfoT> *idol::Heuristics::IntegerMaster<NodeInfoT>::operator()() {

    if (!m_optimizer_factory) {
        throw Exception("No solver was given to solve the integer master problem, please call IntegerMaster.rst::with_osi_interface to configure.");
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

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT> &idol::Heuristics::IntegerMaster<NodeInfoT>::with_optimizer(const OptimizerFactory &t_optimizer) {

    if (m_optimizer_factory) {
        throw Exception("A solver has already been given.");
    }

    m_optimizer_factory.reset(t_optimizer.clone());

    return *this;
}

template<class NodeInfoT>
idol::BranchAndBoundCallbackFactory<NodeInfoT> *idol::Heuristics::IntegerMaster<NodeInfoT>::clone() const {
    return new IntegerMaster(*this);
}

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT> &idol::Heuristics::IntegerMaster<NodeInfoT>::with_integer_columns(bool t_value) {
    m_integer_columns = t_value;
    return *this;
}

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT> &idol::Heuristics::IntegerMaster<NodeInfoT>::with_time_limit(double t_time_limit) {
    m_time_limit = t_time_limit;
    return *this;
}

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT> &idol::Heuristics::IntegerMaster<NodeInfoT>::with_iteration_limit(unsigned int t_iteration_limit) {
    m_iteration_limit = t_iteration_limit;
    return *this;
}

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT> &idol::Heuristics::IntegerMaster<NodeInfoT>::with_max_depth(unsigned int t_max_depth) {
    m_max_depth = t_max_depth;
    return *this;
}

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT> &idol::Heuristics::IntegerMaster<NodeInfoT>::with_frequency(unsigned int t_frequency) {
    m_frequency = t_frequency;
    return *this;
}

template<class NodeInfoT>
idol::Heuristics::IntegerMaster<NodeInfoT>::Strategy::Strategy(const OptimizerFactory &t_optimizer)
        : m_optimizer_factory(t_optimizer.clone()) {

}

template<class NodeInfoT>
void idol::Heuristics::IntegerMaster<NodeInfoT>::Strategy::operator()(CallbackEvent t_event) {

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
    const auto& dantzig_wolfe_optimizer = relaxation.optimizer().template as<Optimizers::DantzigWolfeDecomposition>();
    const auto& original_model = this->original_model();
    const auto& formulation = dantzig_wolfe_optimizer.formulation();
    const unsigned int n_sub_problems = formulation.n_sub_problems();

    std::unique_ptr<Model> integer_master_new(relaxation.clone());
    auto& dw = integer_master_new->optimizer().as<Optimizers::DantzigWolfeDecomposition>();

    // Here, we add all columns currently present in the relaxation
    for (unsigned int k = 0 ; k < n_sub_problems ; ++k) {
        for (const auto& [var, col] : formulation.present_generators(k)) {
            dw.formulation().generate_column(k, col);
        }
    }

    // Set integers
    if (m_integer_columns) {
        for (unsigned int i = 0 ; i < n_sub_problems ; ++i) {
            for (const auto &[alpha, generator]: dw.formulation().present_generators(i)) {
                dw.formulation().master().set_var_type(alpha, Binary);
            }
        }
    }

    // Use different master optimizer
    dw.set_master_optimizer_factory(*m_optimizer_factory);

    // Set parameters
    dw.set_param_logs(false);
    dw.set_param_iteration_limit(0);
    dw.set_param_time_limit(std::min(m_time_limit, relaxation.optimizer().get_remaining_time()));

    // Set bound limit if there is a current incumbent
    auto& branch_and_bound = this->original_model().optimizer().template as<Optimizers::BranchAndBound<NodeInfoT>>();;
    dw.set_param_best_bound_stop(branch_and_bound.get_best_obj());

    integer_master_new->optimize();

    const int status = integer_master_new->get_status();

    if (status != Optimal && status != Feasible) {
        return;
    }

    auto* info = new NodeInfoT();
    info->save(original_model, *integer_master_new);
    this->submit_heuristic_solution(info);

}

#endif //IDOL_INTEGERMASTER_H
