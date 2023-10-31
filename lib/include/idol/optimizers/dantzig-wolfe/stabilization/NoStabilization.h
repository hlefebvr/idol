//
// Created by henri on 31.10.23.
//

#ifndef IDOL_NOSTABILIZATION_H
#define IDOL_NOSTABILIZATION_H

#include "DualPriceSmoothingStabilization.h"

namespace idol::DantzigWolfe {
    class NoStabilization;
}

class idol::DantzigWolfe::NoStabilization : public DualPriceSmoothingStabilization {
public:
    explicit NoStabilization() = default;

    class Strategy : public DualPriceSmoothingStabilization::Strategy {
    public:
        explicit Strategy() = default;

        void initialize() override {
            // intentionally left blank
        }

        void update_stability_center(const Solution::Dual &t_master_dual) override {
            // intentionally left blank
        }

        Solution::Dual compute_smoothed_dual_solution(const Solution::Dual &t_master_dual) override {
            return t_master_dual;
        }

    };

    DualPriceSmoothingStabilization::Strategy *operator()() const override {
        return new Strategy();
    }

    DualPriceSmoothingStabilization *clone() const override {
        return new NoStabilization(*this);
    }
};

#endif //IDOL_NOSTABILIZATION_H
