//
// Created by henri on 19/02/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include <ostream>
#include "../../modeling/variables/Var.h"
#include "../../errors/Exception.h"
#include "backends/Relaxation.h"

class BranchAndBound;
class AbstractModel;

namespace Solution {
    class Primal;
}

namespace impl {
    class Callback;
}

class HeuristicInterface {
    impl::Callback& m_parent;
    Relaxation& m_relaxation;

    std::list<std::tuple<const Req<double, Var>*, Var, double>> m_history_Var_double;
    std::list<std::tuple<const Req<int, Var>*, Var, int>> m_history_Var_int;
public:
    HeuristicInterface(impl::Callback& t_parent, Relaxation& t_relaxation);

    ~HeuristicInterface();

    void temporary_set(const Req<double, Var>& t_attr, const Var& t_var, double t_value);
    void temporary_set(const Req<int, Var>& t_attr, const Var& t_var, int t_value);
    void reoptimize();
};

class impl::Callback {
    friend class ::BranchAndBound;
    friend class HeuristicInterface;
    BranchAndBound* m_parent = nullptr;
protected:
    [[nodiscard]] const Relaxation& relaxation() const;

    [[nodiscard]] const AbstractModel& original_model() const;

    bool submit(Solution::Primal&& t_solution) const;

    HeuristicInterface heuristic_interface();
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
