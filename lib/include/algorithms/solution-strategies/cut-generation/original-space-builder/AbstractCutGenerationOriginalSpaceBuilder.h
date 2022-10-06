//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_ABSTRACTCUTGENERATIONORIGINALSPACEBUILDER_H
#define OPTIMIZE_ABSTRACTCUTGENERATIONORIGINALSPACEBUILDER_H

#include "modeling/solutions/Solution.h"

class CutGenerationSubproblem;

class AbstractCutGenerationOriginalSpaceBuilder {
public:
    virtual ~AbstractCutGenerationOriginalSpaceBuilder() = default;

    [[nodiscard]] virtual Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Solution::Dual &t_rmp_duals) const = 0;
};

#endif //OPTIMIZE_ABSTRACTCUTGENERATIONORIGINALSPACEBUILDER_H
