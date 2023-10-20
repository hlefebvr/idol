//
// Created by henri on 23/03/23.
//

#ifndef IDOL_HIGHS_H
#define IDOL_HIGHS_H

#include "idol/optimizers/OptimizerFactory.h"
#include "idol/containers/Map.h"

namespace idol {
    class HiGHS;
}

class idol::HiGHS : public OptimizerFactoryWithDefaultParameters<HiGHS> {
    std::optional<bool> m_continuous_relaxation;

    explicit HiGHS(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    HiGHS(const HiGHS&) = default;
public:
    HiGHS() = default;
    HiGHS(HiGHS&&) noexcept = default;

    HiGHS& operator=(const HiGHS&) = delete;
    HiGHS& operator=(HiGHS&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static HiGHS ContinuousRelaxation();

    [[nodiscard]] HiGHS *clone() const override;

    HiGHS& with_continuous_relaxation_only(bool t_value);
};

#endif //IDOL_HIGHS_H
