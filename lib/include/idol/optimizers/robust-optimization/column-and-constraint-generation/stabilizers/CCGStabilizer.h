//
// Created by henri on 06.03.24.
//

#ifndef IDOL_CCGSTABILIZER_H
#define IDOL_CCGSTABILIZER_H

namespace idol::Robust {
    class CCGStabilizer;
}

class idol::Robust::CCGStabilizer {
public:
    virtual ~CCGStabilizer() = default;

    class Strategy {
    public:
        virtual ~Strategy() = default;
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual CCGStabilizer* clone() const = 0;
};

#endif //IDOL_CCGSTABILIZER_H
