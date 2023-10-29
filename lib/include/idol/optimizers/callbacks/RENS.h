//
// Created by henri on 16.10.23.
//

#ifndef IDOL_RENS_H
#define IDOL_RENS_H

#include "CallbackFactory.h"
#include "Callback.h"
#include "idol/optimizers/OptimizerFactory.h"

namespace idol::Heuristics {
    class RENS;
}

class idol::Heuristics::RENS : public CallbackFactory {

    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    double m_minimum_ratio_of_integer_variables_to_fix = .5;
    double m_minimum_ratio_of_variables_to_fix = .25;

    RENS(const RENS& t_src);
public:
    RENS() = default;

    class Strategy : public Callback {
        double m_minimum_ratio_of_integer_variables_to_fix;
        double m_minimum_ratio_of_variables_to_fix;
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
        unsigned int m_n_relevant_calls = 0;
        unsigned int m_frequency = 10;
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        Strategy(double t_minimum_ratio_of_integer_variables_to_fix,
                 double t_minimum_ratio_of_variables_to_fix,
                 OptimizerFactory* t_optimizer_factory);
    };

    Callback *operator()() override {

        auto* result = new Strategy(m_minimum_ratio_of_integer_variables_to_fix,
                                    m_minimum_ratio_of_variables_to_fix,
                                    m_optimizer_factory->clone());

        return result;
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new RENS(*this);
    }

    RENS& with_optimizer(const OptimizerFactory& t_optimizer_factory);

};

#endif //IDOL_RENS_H
