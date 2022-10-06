//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_ABSTRACTCUTGENERATIONORIGINALSPACEBUILDER_H
#define OPTIMIZE_ABSTRACTCUTGENERATIONORIGINALSPACEBUILDER_H

#include "modeling/solutions/Solution.h"

class CutGenerationSubproblem;
class Algorithm;

class AbstractCutGenerationOriginalSpaceBuilder {
public:
    virtual ~AbstractCutGenerationOriginalSpaceBuilder() = default;

    [[nodiscard]] virtual Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const = 0;
};

#endif //OPTIMIZE_ABSTRACTCUTGENERATIONORIGINALSPACEBUILDER_H
