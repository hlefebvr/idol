//
// Created by henri on 05/04/23.
//

#ifndef IDOL_KNAPSACKSOLVER_H
#define IDOL_KNAPSACKSOLVER_H

#include "optimizers/OptimizerFactory.h"

class KnapsackSolver : public OptimizerFactoryWithDefaultParameters<KnapsackSolver> {
public:
    KnapsackSolver() = default;
    KnapsackSolver(const KnapsackSolver&) = default;
    KnapsackSolver(KnapsackSolver&&) noexcept = default;

    KnapsackSolver& operator=(const KnapsackSolver&) = delete;
    KnapsackSolver& operator=(KnapsackSolver&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    [[nodiscard]] KnapsackSolver *clone() const override;
};

#endif //IDOL_KNAPSACKSOLVER_H
