//
// Created by henri on 25/11/22.
//

#ifndef IDOL_PARAMETERS_ALGORITHM_H
#define IDOL_PARAMETERS_ALGORITHM_H

#include "Parameters.h"
#include <limits>
#include <thread>

IDOL_CREATE_PARAMETER_CLASS(Algorithm)

IDOL_CREATE_PARAMETER_TYPE(Algorithm, double, 5)
IDOL_CREATE_PARAMETER(Algorithm, double, 0, TimeLimit, std::numeric_limits<double>::max())
IDOL_CREATE_PARAMETER(Algorithm, double, 1, MIPGap, 1e-4)
IDOL_CREATE_PARAMETER(Algorithm, double, 2, CutOff, std::numeric_limits<double>::max())
IDOL_CREATE_PARAMETER(Algorithm, double, 3, BestObjStop, -Inf)
IDOL_CREATE_PARAMETER(Algorithm, double, 4, BestBoundStop, +Inf)

IDOL_CREATE_PARAMETER_TYPE(Algorithm, int, 2)
IDOL_CREATE_PARAMETER(Algorithm, int, 0, MaxIterations, std::numeric_limits<int>::max());
IDOL_CREATE_PARAMETER(Algorithm, int, 1, MaxThreads, std::thread::hardware_concurrency());

IDOL_CREATE_PARAMETER_TYPE(Algorithm, bool, 3)
IDOL_CREATE_PARAMETER(Algorithm, bool, 0, Presolve, true);
IDOL_CREATE_PARAMETER(Algorithm, bool, 1, InfeasibleOrUnboundedInfo, false);
IDOL_CREATE_PARAMETER(Algorithm, bool, 2, ResetBeforeSolving, false);

#endif //IDOL_PARAMETERS_ALGORITHM_H
