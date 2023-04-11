//
// Created by henri on 30/03/23.
//

#ifndef IDOL_INTEGERMASTERHEURISTIC_H
#define IDOL_INTEGERMASTERHEURISTIC_H

#include "optimizers/branch-and-bound/nodes/NodeInfo.h"
#include "optimizers/branch-and-bound/callbacks/CallbackFactory.h"
#include "optimizers/branch-and-bound/callbacks/Callback.h"

class IntegerMasterHeuristic : public CallbackFactory<NodeInfo> {
    std::unique_ptr<OptimizerFactory> m_optimizer_factory;

    std::optional<bool> m_integer_columns;

    IntegerMasterHeuristic(const IntegerMasterHeuristic& t_src);
public:
    IntegerMasterHeuristic() = default;

    IntegerMasterHeuristic(IntegerMasterHeuristic&&) noexcept = default;

    IntegerMasterHeuristic& operator=(const IntegerMasterHeuristic&) = delete;
    IntegerMasterHeuristic& operator=(IntegerMasterHeuristic&&) noexcept = default;

    class Strategy : public Callback<NodeInfo> {
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
        bool m_integer_columns = true;
    protected:
        void operator()(BranchAndBoundEvent t_event) override;
    public:
        explicit Strategy(const OptimizerFactory& t_optimizer);

        [[nodiscard]] bool with_integer_columns() const { return m_integer_columns; }

        void set_integer_columns(bool t_value) { m_integer_columns = t_value; }
    };

    Callback<NodeInfo> *operator()() override;

    [[nodiscard]] CallbackFactory<NodeInfo> *clone() const override;

    IntegerMasterHeuristic& with_solver(const OptimizerFactory& t_optimizer);

    IntegerMasterHeuristic& with_integer_columns(bool t_value);
};

#endif //IDOL_INTEGERMASTERHEURISTIC_H
