//
// Created by henri on 31.10.23.
//

#ifndef IDOL_WENTGES_H
#define IDOL_WENTGES_H

#include <optional>
#include "DualPriceSmoothingStabilization.h"

namespace idol::DantzigWolfe {
    class Wentges;
}

class idol::DantzigWolfe::Wentges : public DualPriceSmoothingStabilization {
    double m_initial_factor;
public:
    explicit Wentges(double t_initial_factor) : m_initial_factor(t_initial_factor) {}

    class Strategy : public DualPriceSmoothingStabilization::Strategy {
        double m_factor;
        std::optional<Solution::Dual> m_stability_center;
    public:
        explicit Strategy(double t_initial_factor) : m_factor(t_initial_factor) {}

        void initialize() override {
            m_stability_center.reset();
        }

        void update_stability_center(const Solution::Dual &t_master_dual) override {
            m_stability_center = t_master_dual;
        }

        Solution::Dual compute_smoothed_dual_solution(const Solution::Dual &t_master_dual) override {

            if (!m_stability_center.has_value() || m_factor <= 1e-4) {
                m_stability_center = t_master_dual;
                return t_master_dual;
            }

            auto result = m_factor * m_stability_center.value() + (1. - m_factor) * t_master_dual;

            m_stability_center = t_master_dual;

            return result;
        }

    };

    DualPriceSmoothingStabilization::Strategy *operator()() const override {
        return new Strategy(m_initial_factor);
    }

    DualPriceSmoothingStabilization *clone() const override {
        return new Wentges(*this);
    }
};

#endif //IDOL_WENTGES_H
