//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_IIS_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_IIS_H

#include "CutGenerationOriginalSpaceBuilder.h"

class Model;


namespace CutGenerationOriginalSpaceBuilders {
    class IIS;
}

class CutGenerationOriginalSpaceBuilders::IIS : public CutGenerationOriginalSpaceBuilder {
    const Model& m_rmp;
public:
    explicit IIS(const Model& t_rmp);
    [[nodiscard]] Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERS_IIS_H
