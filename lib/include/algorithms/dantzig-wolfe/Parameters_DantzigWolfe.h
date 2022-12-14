//
// Created by henri on 14/12/22.
//

#ifndef IDOL_PARAMETERS_DANTZIGWOLFE_H
#define IDOL_PARAMETERS_DANTZIGWOLFE_H

#include "../parameters/Parameters.h"

IDOL_CREATE_PARAMETER_CLASS(DantzigWolfe)

IDOL_CREATE_PARAMETER_TYPE(DantzigWolfe, int, 2)
IDOL_CREATE_PARAMETER(DantzigWolfe, int, 0, CleanUpThreshold, 10000)
IDOL_CREATE_PARAMETER(DantzigWolfe, int, 1, LogFrequency, 10)

IDOL_CREATE_PARAMETER_TYPE(DantzigWolfe, double, 1)
IDOL_CREATE_PARAMETER(DantzigWolfe, double, 0, CleanUpRatio, 2./3.)

#endif //IDOL_PARAMETERS_DANTZIGWOLFE_H
