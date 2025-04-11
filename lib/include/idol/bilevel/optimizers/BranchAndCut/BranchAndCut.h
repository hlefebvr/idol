//
// Created by henri on 11.04.25.
//

#ifndef IDOL_BILEVEL_BRANCHANDCUT_H
#define IDOL_BILEVEL_BRANCHANDCUT_H

#include "idol/general/optimizers/OptimizerFactory.h"
#include "idol/bilevel/optimizers/BilevelOptimizerInterface.h"

namespace idol::Bilevel {
    class BranchAndCut;
}

class idol::Bilevel::BranchAndCut : public OptimizerFactoryWithDefaultParameters<BranchAndCut>, public Bilevel::OptimizerInterface {
    const Bilevel::Description* m_description = nullptr;
    std::unique_ptr<OptimizerFactory> m_optimizer_for_sub_problems;
public:
    BranchAndCut() = default;

    explicit BranchAndCut(const Bilevel::Description& t_description);

    BranchAndCut(const BranchAndCut& t_src);
    BranchAndCut(BranchAndCut&&) noexcept = delete;

    BranchAndCut& operator=(const BranchAndCut&) = delete;
    BranchAndCut& operator=(BranchAndCut&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    BranchAndCut& with_sub_problem_optimizer(const OptimizerFactory& t_optimizer);

    void set_bilevel_description(const Description &t_bilevel_description) override;

    [[nodiscard]] Bilevel::BranchAndCut *clone() const override;
};

#endif //IDOL_BILEVEL_BRANCHANDCUT_H
