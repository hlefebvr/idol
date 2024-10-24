//
// Created by henri on 12/04/23.
//

#ifndef IDOL_CALLBACKFACTORY_H
#define IDOL_CALLBACKFACTORY_H

namespace idol {
    class Callback;
    class CallbackFactory;
}

class idol::CallbackFactory {
public:
    virtual ~CallbackFactory() = default;

    virtual Callback* operator()() = 0;

    [[nodiscard]] virtual CallbackFactory* clone() const = 0;
};

#endif //IDOL_CALLBACKFACTORY_H
