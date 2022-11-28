//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTES_CTR_H
#define IDOL_ATTRIBUTES_CTR_H

#include "../models/Attributes.h"

IDOL_CREATE_ATTRIBUTE_CLASS(Ctr)

IDOL_CREATE_ATTRIBUTE_TYPE(Ctr, IDOL_int, 2)
IDOL_CREATE_ATTRIBUTE(Ctr, IDOL_int, 0, Type)
IDOL_CREATE_ATTRIBUTE(Ctr, IDOL_int, 1, Status)

IDOL_CREATE_ATTRIBUTE_TYPE(Ctr, Row, 1)
IDOL_CREATE_ATTRIBUTE(Ctr, Row, 0, Row)

IDOL_CREATE_ATTRIBUTE_TYPE(Ctr, Constant, 1)
IDOL_CREATE_ATTRIBUTE(Ctr, Constant, 0, Rhs)

#endif //IDOL_ATTRIBUTES_CTR_H
