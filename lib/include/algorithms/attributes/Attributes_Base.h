//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTES_BASE_H
#define OPTIMIZE_ATTRIBUTES_BASE_H

#include "AttributesMacros.h"
#include <thread>

CREATE_ATTRIBUTE_SECTION(Base, 7);
CREATE_ATTRIBUTE(Base, 0, double, TimeLimit, 3600., AttributesSchemaProperty::GreaterThan<0>);
CREATE_ATTRIBUTE(Base, 1, unsigned int, MaxIterations, std::numeric_limits<unsigned int>::max(), AttributesSchemaProperty::GreaterThan<0>);
CREATE_ATTRIBUTE(Base, 2, unsigned int, MaxThreads, std::thread::hardware_concurrency(), AttributesSchemaProperty::GreaterThan<0>);
CREATE_ATTRIBUTE(Base, 3, double, MipGap, 1e-4, AttributesSchemaProperty::GreaterThan<0>);
CREATE_ATTRIBUTE(Base, 4, bool, Presolve, true);
CREATE_ATTRIBUTE(Base, 5, bool, InfeasibleOrUnboundedInfo, false);
CREATE_ATTRIBUTE(Base, 6, double, CutOff, Inf);

#endif //OPTIMIZE_ATTRIBUTES_BASE_H
