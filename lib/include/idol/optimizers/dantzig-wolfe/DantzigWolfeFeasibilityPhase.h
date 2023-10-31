//
// Created by henri on 31.10.23.
//

#ifndef IDOL_DANTZIGWOLFEFEASIBILITYPHASE_H
#define IDOL_DANTZIGWOLFEFEASIBILITYPHASE_H

namespace idol::DantzigWolfe {
    class FeasibilityPhase;
}

class idol::DantzigWolfe::FeasibilityPhase {
public:
    virtual ~FeasibilityPhase() = default;

    class Strategy {
    public:
        virtual ~Strategy() = default;
    };

    virtual Strategy* operator()() const = 0;

    [[nodiscard]] virtual FeasibilityPhase* clone() const = 0;
};

#endif //IDOL_DANTZIGWOLFEFEASIBILITYPHASE_H
