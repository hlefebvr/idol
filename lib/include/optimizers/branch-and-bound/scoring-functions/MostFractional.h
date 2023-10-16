//
// Created by henri on 16.10.23.
//

#ifndef IDOL_MOSTFRACTIONAL_H
#define IDOL_MOSTFRACTIONAL_H

#include "VariableScoringFunctionFactory.h"
#include "VariableScoringFunction.h"

namespace idol {
    class MostFractional;
}

class idol::MostFractional : public VariableScoringFunctionFactory {
public:

    class Strategy : public VariableScoringFunction {
    public:
        std::list<double> operator()(const std::list<Var> &t_variables, const Solution::Primal &t_primal_values) override;
    };

    Strategy *operator()() override {
        return new Strategy();
    }

    MostFractional *clone() const override {
        return new MostFractional(*this);
    }
};

std::list<double> idol::MostFractional::Strategy::operator()(const std::list<Var> &t_variables,
                                                             const idol::Solution::Primal &t_primal_values) {

    std::list<double> result;

    for (const auto& var : t_variables) {
        const double value = t_primal_values.get(var);
        const double score = std::abs(value - std::round(value));
        result.emplace_back(score);
    }

    return result;
}

#endif //IDOL_MOSTFRACTIONAL_H
