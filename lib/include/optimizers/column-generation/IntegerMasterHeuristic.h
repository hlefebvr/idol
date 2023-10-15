//
// Created by henri on 30/03/23.
//

#ifndef IDOL_INTEGERMASTERHEURISTIC_H
#define IDOL_INTEGERMASTERHEURISTIC_H

#include "optimizers/branch-and-bound/nodes/NodeInfo.h"
#include "optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"

namespace idol {
    class IntegerMasterHeuristic;
}

class idol::IntegerMasterHeuristic : public BranchAndBoundCallbackFactory<NodeInfo> {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    std::optional<bool> m_integer_columns;

    std::optional<double> m_time_limit;
    std::optional<unsigned int> m_iteration_limit;

    IntegerMasterHeuristic(const IntegerMasterHeuristic& t_src);
public:
    IntegerMasterHeuristic() = default;

    IntegerMasterHeuristic(IntegerMasterHeuristic&&) noexcept = default;

    IntegerMasterHeuristic& operator=(const IntegerMasterHeuristic&) = delete;
    IntegerMasterHeuristic& operator=(IntegerMasterHeuristic&&) noexcept = default;

    class Strategy : public BranchAndBoundCallback<NodeInfo> {
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
        bool m_integer_columns = true;
        double m_time_limit = std::numeric_limits<double>::max();
        unsigned int m_iteration_limit = 200;
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        explicit Strategy(const OptimizerFactory& t_optimizer);

        [[nodiscard]] bool with_integer_columns() const { return m_integer_columns; }

        void set_integer_columns(bool t_value) { m_integer_columns = t_value; }

        void set_time_limit(double t_time_limit) { m_time_limit = std::max(0., t_time_limit); }

        void set_iteration_limit(unsigned int t_iteration_limit) { m_iteration_limit = t_iteration_limit; }
    };

    BranchAndBoundCallback<NodeInfo> *operator()() override;

    [[nodiscard]] BranchAndBoundCallbackFactory<NodeInfo> *clone() const override;

    IntegerMasterHeuristic& with_optimizer(const OptimizerFactory& t_optimizer);

    IntegerMasterHeuristic& with_integer_columns(bool t_value);
};

#endif //IDOL_INTEGERMASTERHEURISTIC_H
