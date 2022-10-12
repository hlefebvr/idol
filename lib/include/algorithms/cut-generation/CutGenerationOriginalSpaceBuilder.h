//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDER_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDER_H

#include "../../modeling/solutions/Solution.h"

class CutGenerationSP;
class Algorithm;

class CutGenerationOriginalSpaceBuilder {
public:
    virtual ~CutGenerationOriginalSpaceBuilder() = default;

    [[nodiscard]] virtual Solution::Primal primal_solution(const CutGenerationSP &t_subproblem, const Algorithm &t_rmp_solution_strategy) const = 0;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDER_H
