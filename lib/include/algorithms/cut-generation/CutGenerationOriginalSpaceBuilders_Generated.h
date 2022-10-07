//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_GENERATED_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_GENERATED_H

#include "CutGenerationOriginalSpaceBuilder.h"

namespace CutGenerationOriginalSpaceBuilders {
    class Generated;
}

class CutGenerationOriginalSpaceBuilders::Generated : public CutGenerationOriginalSpaceBuilder {
public:
    [[nodiscard]] Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_GENERATED_H
