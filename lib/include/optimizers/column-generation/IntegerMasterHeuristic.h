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

    IntegerMasterHeuristic(const IntegerMasterHeuristic& t_src);
public:
    IntegerMasterHeuristic() = default;

    IntegerMasterHeuristic(IntegerMasterHeuristic&&) noexcept = default;

    IntegerMasterHeuristic& operator=(const IntegerMasterHeuristic&) = delete;
    IntegerMasterHeuristic& operator=(IntegerMasterHeuristic&&) noexcept = default;

    class Strategy : public Callback<NodeInfo> {
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    protected:
        void operator()(BranchAndBoundEvent t_event) override;
    public:
        explicit Strategy(const OptimizerFactory& t_optimizer);
    };

    Callback<NodeInfo> *operator()() override;

    CallbackFactory<NodeInfo> *clone() const override;

    IntegerMasterHeuristic& with_solver(const OptimizerFactory& t_optimizer);
};

#endif //IDOL_INTEGERMASTERHEURISTIC_H
