//
// Created by henri on 21/12/22.
//

#ifndef IDOL_CALLBACKS_INTEGERMASTERPROBLEM_H
#define IDOL_CALLBACKS_INTEGERMASTERPROBLEM_H

#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "UserCallback.h"
#include "algorithms/branch-and-bound/BranchAndBound_Events.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"

namespace Callbacks {
    class IntegerMasterProblem;
}

class Callbacks::IntegerMasterProblem : public UserCallback<BranchAndBound> {
public:
    void execute(const EventType &t_event) override {

        if (t_event != Event_::BranchAndBound::RelaxationSolved) {
            return;
        }

        auto& solver = parent().solution_strategy().as<DantzigWolfe>();
        auto& master = solver.master_solution_strategy();

        std::list<Var> m_integer_variables;

        for (const auto& subproblem : solver.subproblems()) {
            for (const auto& [var, generator] : subproblem.present_generators()) {
                master.set(Attr::Var::Type, var, Integer);
                m_integer_variables.emplace_back(var);
            }
        }

        master.solve();

        if (master.status() != Optimal || master.status() != Feasible) {
            throw Exception("Unexpected master problem status.");
        }

        parent().submit_solution(master.primal_solution());

        for (const auto& var : m_integer_variables) {
            master.set(Attr::Var::Type, var, Continuous);
        }

    }
};

#endif //IDOL_CALLBACKS_INTEGERMASTERPROBLEM_H
