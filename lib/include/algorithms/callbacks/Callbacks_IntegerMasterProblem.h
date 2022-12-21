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
    std::list<Var> m_integer_variables;
public:

    void execute(const EventType &t_event) override {

        if (t_event != Event_::BranchAndBound::RelaxationSolved) {
            return;
        }

        auto& solver = parent().solution_strategy().as<DantzigWolfe>();
        auto& master = solver.master_solution_strategy();

        for (const auto& subproblem : solver.subproblems()) {
            for (const auto& [var, generator] : subproblem.present_generators()) {
                master.set(Attr::Var::Type, var, Binary);
                m_integer_variables.emplace_back(var);
            }
        }

        master.set(Param::Algorithm::BestBoundStop, parent().upper_bound());
        master.set(Param::Algorithm::TimeLimit, 600);
        master.solve();

        const auto status = master.status();

        if (status == Optimal || status == Feasible) {
            parent().submit_solution(master.primal_solution());
        } else if (status != Infeasible) {
            throw Exception("Unexpected master problem status.");
        }

        for (const auto& var : m_integer_variables) {
            master.set(Attr::Var::Type, var, Continuous);
        }

        m_integer_variables.clear();


    }
};

#endif //IDOL_CALLBACKS_INTEGERMASTERPROBLEM_H
