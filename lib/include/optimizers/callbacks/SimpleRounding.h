//
// Created by henri on 16.10.23.
//

#ifndef IDOL_SIMPLEROUNDING_H
#define IDOL_SIMPLEROUNDING_H

#include "CallbackFactory.h"
#include "Callback.h"

namespace idol::Heuristics {
    class SimpleRounding;
}

class idol::Heuristics::SimpleRounding : public CallbackFactory {

    SimpleRounding(const SimpleRounding& t_src) = default;
public:
    SimpleRounding() = default;

    class Strategy : public Callback {
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
    };

    Callback *operator()() override {

        auto* result = new Strategy();

        return result;
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new SimpleRounding(*this);
    }

};

#endif //IDOL_SIMPLEROUNDING_H
