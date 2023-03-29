//
// Created by henri on 23/03/23.
//

#ifndef IDOL_MOSEK_H
#define IDOL_MOSEK_H

#include "../OptimizerFactory.h"

class Mosek : public OptimizerFactoryWithDefaultParameters<Mosek> {
    bool m_continuous_relaxation = false;

    explicit Mosek(bool t_continuous_relaxation) : m_continuous_relaxation(t_continuous_relaxation) {}
    Mosek(const Mosek&) = default;
public:
    Mosek() = default;
    Mosek(Mosek&&) noexcept = default;

    Mosek& operator=(const Mosek&) = delete;
    Mosek& operator=(Mosek&&) noexcept = delete;

    Optimizer *operator()(const Model &t_model) const override;

    static Mosek ContinuousRelaxation();

    [[nodiscard]] Mosek *clone() const override;
};

#endif //IDOL_MOSEK_H
