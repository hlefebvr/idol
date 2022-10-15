//
// Created by henri on 14/10/22.
//

#ifndef IDOL_ATTRIBUTES_ROWGENERATION_H
#define IDOL_ATTRIBUTES_ROWGENERATION_H

#include "../attributes/AttributesMacros.h"

CREATE_ATTRIBUTE_SECTION(RowGeneration, 2);
CREATE_ATTRIBUTE(RowGeneration, 0, bool, SubOptimalRMP, false);
CREATE_ATTRIBUTE(RowGeneration, 1, double, SubOptimalRMP_Tolerance, 2e-1);

#endif //IDOL_ATTRIBUTES_ROWGENERATION_H
