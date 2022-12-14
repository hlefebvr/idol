//
// Created by henri on 14/12/22.
//

#ifndef IDOL_PARAMETERS_DANTZIGWOLFE_H
#define IDOL_PARAMETERS_DANTZIGWOLFE_H

#include "../parameters/Parameters.h"

IDOL_CREATE_PARAMETER_CLASS(DantzigWolfe)

IDOL_CREATE_PARAMETER_TYPE(DantzigWolfe, bool, 1)
IDOL_CREATE_PARAMETER(DantzigWolfe, bool, 0, BranchingOnMaster, 1)

#endif //IDOL_PARAMETERS_DANTZIGWOLFE_H
