//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DUALPRICESMOOTHINGSTABILIZATION_H
#define IDOL_DUALPRICESMOOTHINGSTABILIZATION_H

#include "idol/modeling/solutions/Solution.h"

namespace idol::DantzigWolfe {
    class DualPriceSmoothingStabilization;
}

class idol::DantzigWolfe::DualPriceSmoothingStabilization {
public:
    virtual ~DualPriceSmoothingStabilization() = default;

    class Strategy {
    public:
        virtual ~Strategy() = default;

        virtual void initialize() = 0;

        virtual void update_stability_center(const Solution::Dual& t_master_dual) = 0;

        virtual Solution::Dual compute_smoothed_dual_solution(const Solution::Dual& t_master_dual) = 0;
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual DualPriceSmoothingStabilization* clone() const = 0;
};

#endif //IDOL_DUALPRICESMOOTHINGSTABILIZATION_H
