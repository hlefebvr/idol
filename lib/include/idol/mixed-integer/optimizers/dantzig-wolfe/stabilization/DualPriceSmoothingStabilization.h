//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DUALPRICESMOOTHINGSTABILIZATION_H
#define IDOL_DUALPRICESMOOTHINGSTABILIZATION_H

#include "idol/general/utils/Point.h"

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

        virtual void update_stability_center(const DualPoint& t_master_dual) = 0;

        virtual DualPoint compute_smoothed_dual_solution(const DualPoint& t_master_dual) = 0;

        // Returns true iff the last call to compute_smoothed_dual_solution returned
        // the master dual unchanged. In that case, pricing with the smoothed dual
        // produces a valid Lagrangian bound (as if raw-dual pricing was performed).
        // Conservative default: assume the stabilisation does modify the dual.
        [[nodiscard]] virtual bool last_result_equals_master_dual() const { return false; }
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual DualPriceSmoothingStabilization* clone() const = 0;
};

#endif //IDOL_DUALPRICESMOOTHINGSTABILIZATION_H
