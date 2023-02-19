//
// Created by henri on 19/02/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include <ostream>
#include "../../errors/Exception.h"

class BranchAndBound;
class AbstractModel;

namespace Solution {
    class Primal;
}

namespace impl {
    class Callback;
}

class impl::Callback {
    friend class ::BranchAndBound;
    BranchAndBound* m_parent = nullptr;
protected:
    [[nodiscard]] const AbstractModel& node_model() const;

    [[nodiscard]] const AbstractModel& original_model() const;

    bool submit(Solution::Primal&& t_solution) const;
};

class Callback : public impl::Callback {
public:
    enum Event { NodeSolved, IncumbentFound };

    virtual ~Callback() = default;

    virtual void execute(Event t_event) = 0;
};

static std::ostream &operator<<(std::ostream& t_os, Callback::Event t_event) {

    switch (t_event) {
        case Callback::NodeSolved: return t_os << "NodeSolved";
        case Callback::IncumbentFound: return t_os << "IncumbentFound";
        default:;
    }

    throw Exception("Enum out of bounds.");
}

#endif //IDOL_CALLBACK_H
