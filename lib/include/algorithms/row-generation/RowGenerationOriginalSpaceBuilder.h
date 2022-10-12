//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDER_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDER_H

#include "../../modeling/solutions/Solution.h"

class RowGenerationSP;
class Algorithm;

class RowGenerationOriginalSpaceBuilder {
public:
    virtual ~RowGenerationOriginalSpaceBuilder() = default;

    [[nodiscard]] virtual Solution::Primal primal_solution(const RowGenerationSP &t_subproblem, const Algorithm &t_rmp_solution_strategy) const = 0;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDER_H
