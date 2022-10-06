//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERDUAL_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERDUAL_H

#include "AbstractCutGenerationOriginalSpaceBuilder.h"

class CutGenerationOriginalSpaceBuilderDual : public AbstractCutGenerationOriginalSpaceBuilder {
public:
    [[nodiscard]] Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERDUAL_H
