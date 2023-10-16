//
// Created by henri on 16.10.23.
//

#ifndef IDOL_UNIFORMLYRANDOM_H
#define IDOL_UNIFORMLYRANDOM_H

#include <random>
#include "VariableScoringFunctionFactory.h"
#include "VariableScoringFunction.h"

namespace idol {
    class UniformlyRandom;
}

class idol::UniformlyRandom : public VariableScoringFunctionFactory {
    std::optional<unsigned int> m_seed;
public:

    class Strategy : public VariableScoringFunction {
        unsigned int m_seed;
    public:
        Strategy(unsigned int t_seed) : m_seed(t_seed) {}

        std::list<double> operator()(const std::list<Var> &t_variables, const Solution::Primal &t_primal_values) override {

            std::mt19937 engine(m_seed);
            std::uniform_real_distribution<double> distribution(0, 1);

            std::list<double> result;

            for (const auto& var : t_variables) {
                result.emplace_back(distribution(engine));
            }

            return result;
        }
    };

    Strategy *operator()() override {

        unsigned int seed = m_seed.has_value() ? m_seed.value() : (std::random_device())();

        return new Strategy(seed);
    }

    UniformlyRandom *clone() const override {
        return new UniformlyRandom(*this);
    }
};

#endif //IDOL_UNIFORMLYRANDOM_H
