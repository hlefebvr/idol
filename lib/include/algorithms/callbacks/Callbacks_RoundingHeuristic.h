//
// Created by henri on 17/10/22.
//

#ifndef IDOL_CALLBACKS_ROUNDINGHEURISTIC_H
#define IDOL_CALLBACKS_ROUNDINGHEURISTIC_H

#include "../branch-and-bound/BranchAndBound.h"
#include <vector>

namespace Callbacks {
    class RoundingHeuristic;
}

class Callbacks::RoundingHeuristic : public BranchAndBound::Callback {
    std::vector<Var> m_branching_candidates;
public:
    explicit RoundingHeuristic(std::vector<Var> t_branching_candidates) : m_branching_candidates(std::move(t_branching_candidates)) {}

    void execute(Context &t_ctx) override {

        Solution::Primal primals = t_ctx.node_primal_solution();

        std::list<std::pair<Var, double>> rounded;
        for (const auto& var : m_branching_candidates) {
            rounded.emplace_back(var, std::round(primals.get(var)));
        }

        advanced(t_ctx).fix_variables(rounded);
        advanced(t_ctx).resolve();
        auto result = advanced(t_ctx).primal_solution();

        if (result.status() == Optimal) {
            t_ctx.submit_solution(std::move(result));
        }

    }
};

#endif //IDOL_CALLBACKS_ROUNDINGHEURISTIC_H
