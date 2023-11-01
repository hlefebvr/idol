//
// Created by henri on 31.10.23.
//

#ifndef IDOL_NEAME_H
#define IDOL_NEAME_H

#include "DualPriceSmoothingStabilization.h"

namespace idol::DantzigWolfe {
    class Neame;
}

class idol::DantzigWolfe::Neame : public DualPriceSmoothingStabilization {
    double m_initial_factor;
public:
    explicit Neame(double t_initial_factor) : m_initial_factor(t_initial_factor) {}

    class Strategy : public DualPriceSmoothingStabilization::Strategy {
        double m_factor;
        std::optional<Solution::Dual> m_smoothed_dual;
    public:
        explicit Strategy(double t_initial_factor) : m_factor(t_initial_factor) {}

        void initialize() override {
            m_smoothed_dual.reset();
        }

        void update_stability_center(const Solution::Dual &t_master_dual) override {
            // intentionally left blank
        }

        Solution::Dual compute_smoothed_dual_solution(const Solution::Dual &t_master_dual) override {

            if (!m_smoothed_dual.has_value() || m_factor <= 1e-4) {
                m_smoothed_dual = t_master_dual;
            } else {
                m_smoothed_dual = m_factor * m_smoothed_dual.value() + (1. - m_factor) * t_master_dual;
            }

            return m_smoothed_dual.value();
        }

    };

    DualPriceSmoothingStabilization::Strategy *operator()() const override {
        return new Strategy(m_initial_factor);
    }

    DualPriceSmoothingStabilization *clone() const override {
        return new Neame(*this);
    }
};

#endif //IDOL_NEAME_H
