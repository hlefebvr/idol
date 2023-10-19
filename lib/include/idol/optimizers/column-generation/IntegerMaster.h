//
// Created by henri on 30/03/23.
//

#ifndef IDOL_INTEGERMASTER_H
#define IDOL_INTEGERMASTER_H

#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"

namespace idol::Heuristics {
    class IntegerMaster;
}

class idol::Heuristics::IntegerMaster : public BranchAndBoundCallbackFactory<NodeVarInfo> {
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

    class Strategy : public BranchAndBoundCallback<NodeVarInfo> {
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

    BranchAndBoundCallback<NodeVarInfo> *operator()() override;

    [[nodiscard]] BranchAndBoundCallbackFactory<NodeVarInfo> *clone() const override;

    IntegerMaster& with_optimizer(const OptimizerFactory& t_optimizer);

    IntegerMaster& with_integer_columns(bool t_value);

    IntegerMaster& with_time_limit(double t_time_limit);

    IntegerMaster& with_iteration_limit(unsigned int t_iteration_limit);

    IntegerMaster& with_max_depth(unsigned int t_max_depth);

    IntegerMaster& with_frequency(unsigned int t_frequency);
};

#endif //IDOL_INTEGERMASTER_H
