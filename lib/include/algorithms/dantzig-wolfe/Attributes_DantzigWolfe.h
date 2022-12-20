//
// Created by henri on 20/12/22.
//

#ifndef IDOL_ATTRIBUTES_DANTZIGWOLFE_H
#define IDOL_ATTRIBUTES_DANTZIGWOLFE_H

#include "../../modeling/models/AttributeManager.h"

IDOL_CREATE_ATTRIBUTE_CLASS(DantzigWolfe)

IDOL_CREATE_ATTRIBUTE_TYPE(DantzigWolfe, IDOL_double, 5)
IDOL_CREATE_ATTRIBUTE(DantzigWolfe, IDOL_double, 0, BestLb, void)
IDOL_CREATE_ATTRIBUTE(DantzigWolfe, IDOL_double, 1, BestUb, void)
IDOL_CREATE_ATTRIBUTE(DantzigWolfe, IDOL_double, 2, LastLb, void)
IDOL_CREATE_ATTRIBUTE(DantzigWolfe, IDOL_double, 3, RelativeGap, void)
IDOL_CREATE_ATTRIBUTE(DantzigWolfe, IDOL_double, 4, AbsoluteGap, void)

#endif //IDOL_ATTRIBUTES_DANTZIGWOLFE_H
