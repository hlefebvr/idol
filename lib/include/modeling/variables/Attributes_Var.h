//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTES_VAR_H
#define IDOL_ATTRIBUTES_VAR_H

#include "../attributes/Attributes.h"

IDOL_CREATE_ATTRIBUTE_CLASS(Var)

IDOL_CREATE_ATTRIBUTE_TYPE(Var, IDOL_double, 2)
IDOL_CREATE_ATTRIBUTE(Var, IDOL_double, 0, Lb, ::Var)
IDOL_CREATE_ATTRIBUTE(Var, IDOL_double, 1, Ub, ::Var)

IDOL_CREATE_ATTRIBUTE_TYPE(Var, IDOL_int, 2)
IDOL_CREATE_ATTRIBUTE(Var, IDOL_int, 0, Type, ::Var)
IDOL_CREATE_ATTRIBUTE(Var, IDOL_int, 1, Index, ::Var)

IDOL_CREATE_ATTRIBUTE_TYPE(Var, Column, 1)
IDOL_CREATE_ATTRIBUTE(Var, Column, 0, Column, ::Var)

IDOL_CREATE_ATTRIBUTE_TYPE(Var, Constant, 1)
IDOL_CREATE_ATTRIBUTE(Var, Constant, 0, Obj, ::Var)

#endif //IDOL_ATTRIBUTES_VAR_H
