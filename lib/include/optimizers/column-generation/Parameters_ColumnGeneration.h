//
// Created by henri on 14/12/22.
//

#ifndef IDOL_PARAMETERS_COLUMNGENERATION_H
#define IDOL_PARAMETERS_COLUMNGENERATION_H

#include "../parameters/Parameters.h"

IDOL_CREATE_PARAMETER_CLASS(ColumnGeneration)

IDOL_CREATE_PARAMETER_TYPE(ColumnGeneration, int, 3)
IDOL_CREATE_PARAMETER(ColumnGeneration, int, 0, CleanUpThreshold, 10000)
IDOL_CREATE_PARAMETER(ColumnGeneration, int, 1, LogFrequency, 10)
IDOL_CREATE_PARAMETER(ColumnGeneration, int, 2, NumParallelPricing, 1)

IDOL_CREATE_PARAMETER_TYPE(ColumnGeneration, bool, 2)
IDOL_CREATE_PARAMETER(ColumnGeneration, bool, 0, FarkasPricing, false)
IDOL_CREATE_PARAMETER(ColumnGeneration, bool, 1, BranchingOnMaster, true)

IDOL_CREATE_PARAMETER_TYPE(ColumnGeneration, double, 3)
IDOL_CREATE_PARAMETER(ColumnGeneration, double, 0, CleanUpRatio, 2./3.)
IDOL_CREATE_PARAMETER(ColumnGeneration, double, 1, ArtificialVarCost, 1e+7)
IDOL_CREATE_PARAMETER(ColumnGeneration, double, 2, SmoothingFactor, 0.)

#endif //IDOL_PARAMETERS_COLUMNGENERATION_H
