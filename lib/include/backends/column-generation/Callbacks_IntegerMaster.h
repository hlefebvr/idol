//
// Created by henri on 19/02/23.
//

#ifndef IDOL_CALLBACKS_INTEGERMASTER_H
#define IDOL_CALLBACKS_INTEGERMASTER_H

#include "backends/callback/Callback.h"
#include "ColumnGeneration.h"

#include <memory>

namespace Callbacks::BranchAndPrice {
    template<class BackendT> class IntegerMaster;
}

template<class BackendT>
class Callbacks::BranchAndPrice::IntegerMaster : public Callback {
public:
    void execute(Event t_event) override {

        if (t_event != Event::NodeSolved) {
            return;
        }

        const auto& node = relaxation().model();

        const auto& column_generation = node.backend().template as<ColumnGeneration>();

        auto integer_master = std::unique_ptr<AbstractModel>(column_generation.master().clone());

        for (const auto& subproblem : column_generation.subproblems()) {
            for (const auto &[alpha, generator]: subproblem.present_generators()) {
                integer_master->set(Attr::Var::Type, alpha, Binary);
            }
        }

        Idol::set_optimizer<BackendT>(*integer_master);

        integer_master->set(Param::Algorithm::BestObjStop, node.get(Param::Algorithm::BestObjStop));
        integer_master->set(Param::Algorithm::TimeLimit, original_model().remaining_time());

        integer_master->optimize();

        const int status = integer_master->get(Attr::Solution::Status);

        if (status == Fail) {
            idol_Log(Warn, BranchAndBound, "Heuristic returned status \"Fail\".");
        }

        if (status != Optimal) {
            return;
        }

        auto solution = save(*integer_master, Attr::Solution::Primal);

        for (const auto& subproblem : column_generation.subproblems()) {
            for (const auto &[alpha, generator]: subproblem.present_generators()) {
                if (solution.get(alpha) > .5) {
                    solution.set(alpha, 0.);
                    solution += generator;
                }
            }
        }

        submit(std::move(solution));

    }
};

#endif //IDOL_CALLBACKS_INTEGERMASTER_H
