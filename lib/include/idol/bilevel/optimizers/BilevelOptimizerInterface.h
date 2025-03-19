//
// Created by henri on 05.02.25.
//

#ifndef IDOL_BILEVELOPTIMIZERINTERFACE_H
#define IDOL_BILEVELOPTIMIZERINTERFACE_H

namespace idol::Bilevel {
    class Description;
    class OptimizerInterface;
}

class idol::Bilevel::OptimizerInterface {
public:
    virtual ~OptimizerInterface() = default;

    virtual void set_bilevel_description(const Bilevel::Description& t_bilevel_description) = 0;
};

#endif //IDOL_BILEVELOPTIMIZERINTERFACE_H
