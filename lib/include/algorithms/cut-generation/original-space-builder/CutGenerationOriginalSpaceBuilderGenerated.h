//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERGENERATED_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERGENERATED_H

#include "AbstractCutGenerationOriginalSpaceBuilder.h"

class CutGenerationOriginalSpaceBuilderGenerated : public AbstractCutGenerationOriginalSpaceBuilder {
public:
    [[nodiscard]] Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERGENERATED_H
