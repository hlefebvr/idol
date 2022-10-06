//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTES_BASE_H
#define OPTIMIZE_ATTRIBUTES_BASE_H

#include "AttributesMacros.h"

CREATE_ATTRIBUTE_SECTION(Base, 3);
CREATE_ATTRIBUTE(Base, 0, double, TimeLimit, 3600., AttributesSchemaProperty::GreaterThan<0>);
CREATE_ATTRIBUTE(Base, 1, bool, Presolve, true);
CREATE_ATTRIBUTE(Base, 2, bool, InfeasibleOrUnboundedInfo, false);

#endif //OPTIMIZE_ATTRIBUTES_BASE_H
