//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_BASE_H
#define OPTIMIZE_BASE_H

#include "attribute_tools.h"

CREATE_ATTRIBUTE_TYPE(Base, 3);
CREATE_ATTRIBUTE(Base, 0, double, TimeLimit, 3600., AttrSchema::GreaterThan<0>);
CREATE_ATTRIBUTE(Base, 1, bool, Presolve, true);
CREATE_ATTRIBUTE(Base, 2, bool, InfeasibleOrUnboundedInfo, false);

#endif //OPTIMIZE_BASE_H
