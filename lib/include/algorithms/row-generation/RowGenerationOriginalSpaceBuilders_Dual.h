//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_DUAL_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_DUAL_H

#include "RowGenerationOriginalSpaceBuilder.h"

namespace RowGenerationOriginalSpaceBuilders {
    class Dual;
}

class RowGenerationOriginalSpaceBuilders::Dual : public RowGenerationOriginalSpaceBuilder {
public:
    [[nodiscard]] Solution::Primal primal_solution(const RowGenerationSP &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_DUAL_H
