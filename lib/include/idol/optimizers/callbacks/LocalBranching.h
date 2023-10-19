//
// Created by henri on 16.10.23.
//

#ifndef IDOL_LOCAL_BRANCHING_H
#define IDOL_LOCAL_BRANCHING_H

#include "CallbackFactory.h"
#include "Callback.h"
#include "idol/optimizers/OptimizerFactory.h"

namespace idol::Heuristics {
    class LocalBranching;
}

class idol::Heuristics::LocalBranching : public CallbackFactory {

    std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    double m_initial_neighbourhood_size = 2;

    LocalBranching(const LocalBranching& t_src);
public:
    LocalBranching() = default;

    class Strategy : public Callback {
        unsigned int m_neighbourhood_size;
        unsigned int m_initial_neighbourhood_size;
        bool m_execute = true;
        std::unique_ptr<OptimizerFactory> m_optimizer_factory;
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        Strategy(unsigned int t_neighbourhood_size,
                 OptimizerFactory* t_optimizer_factory);
    };

    Callback *operator()() override {

        auto* result = new Strategy(m_initial_neighbourhood_size,
                                    m_optimizer_factory->clone());

        return result;
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new LocalBranching(*this);
    }

    LocalBranching& with_optimizer(const OptimizerFactory& t_optimizer_factory);

};

#endif //IDOL_LOCAL_BRANCHING_H
