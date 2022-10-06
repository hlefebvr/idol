//
// Created by henri on 06/10/22.
//

#ifndef OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERIIS_H
#define OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERIIS_H

#include "AbstractCutGenerationOriginalSpaceBuilder.h"

class Model;

class CutGenerationOriginalSpaceBuilderIIS : public AbstractCutGenerationOriginalSpaceBuilder {
    const Model& m_rmp;
public:
    explicit CutGenerationOriginalSpaceBuilderIIS(const Model& t_rmp);
    [[nodiscard]] Solution::Primal primal_solution(const CutGenerationSubproblem &t_subproblem, const Algorithm &t_rmp_solution_strategy) const override;
};

#endif //OPTIMIZE_CUTGENERATIONORIGINALSPACEBUILDERIIS_H
