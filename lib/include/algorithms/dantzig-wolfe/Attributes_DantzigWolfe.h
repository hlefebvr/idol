//
// Created by henri on 20/12/22.
//

#ifndef IDOL_ATTRIBUTES_DANTZIGWOLFE_H
#define IDOL_ATTRIBUTES_DANTZIGWOLFE_H

#include "../../modeling/models/AttributeManager.h"

IDOL_CREATE_ATTRIBUTE_CLASS(Algorithm)

IDOL_CREATE_ATTRIBUTE_TYPE(Algorithm, IDOL_double, 6)
IDOL_CREATE_ATTRIBUTE(Algorithm, IDOL_double, 0, BestLb, void)
IDOL_CREATE_ATTRIBUTE(Algorithm, IDOL_double, 1, BestUb, void)
IDOL_CREATE_ATTRIBUTE(Algorithm, IDOL_double, 2, IterLb, void)
IDOL_CREATE_ATTRIBUTE(Algorithm, IDOL_double, 3, IterUb, void)
IDOL_CREATE_ATTRIBUTE(Algorithm, IDOL_double, 4, RelativeGap, void)
IDOL_CREATE_ATTRIBUTE(Algorithm, IDOL_double, 5, AbsoluteGap, void)

#endif //IDOL_ATTRIBUTES_DANTZIGWOLFE_H
