//
// Created by henri on 28/11/22.
//

#ifndef IDOL_ATTRIBUTES_MODEL_H
#define IDOL_ATTRIBUTES_MODEL_H

#include "modeling/attributes/Attributes.h"
#include "modeling/solutions/Solution.h"

namespace Solutions {
    class Primal;
    class Dual;
}

// RHS
IDOL_CREATE_ATTRIBUTE_CLASS(Rhs)

IDOL_CREATE_ATTRIBUTE_TYPE(Rhs, Rhs, 1)
IDOL_CREATE_ATTRIBUTE(Rhs, Rhs, 0, Expr, void)


// OBJ
IDOL_CREATE_ATTRIBUTE_CLASS(Obj)

IDOL_CREATE_ATTRIBUTE_TYPE(Obj, Obj, 1)
IDOL_CREATE_ATTRIBUTE(Obj, Obj, 0, Expr, void)

IDOL_CREATE_ATTRIBUTE_TYPE(Obj, Constant, 1)
IDOL_CREATE_ATTRIBUTE(Obj, Constant, 0, Const, void)

IDOL_CREATE_ATTRIBUTE_TYPE(Obj, IDOL_int, 1)
IDOL_CREATE_ATTRIBUTE(Obj, IDOL_int, 0, Sense, void)

// MATRIX
IDOL_CREATE_ATTRIBUTE_CLASS(Matrix)

IDOL_CREATE_ATTRIBUTE_TYPE(Matrix, Constant, 1)
IDOL_CREATE_ATTRIBUTE(Matrix, Constant, 0, Coeff, ::Ctr, ::Var)

// Solution
IDOL_CREATE_ATTRIBUTE_CLASS(Solution)

IDOL_CREATE_ATTRIBUTE_TYPE(Solution, IDOL_double, 1)
IDOL_CREATE_ATTRIBUTE(Solution, IDOL_double, 0, ObjVal, void)

IDOL_CREATE_ATTRIBUTE_TYPE(Solution, IDOL_Solution_Primal, 2)
IDOL_CREATE_ATTRIBUTE(Solution, IDOL_Solution_Primal, 0, Primals, void)
IDOL_CREATE_ATTRIBUTE(Solution, IDOL_Solution_Primal, 1, Ray, void)

IDOL_CREATE_ATTRIBUTE_TYPE(Solution, IDOL_Solution_Dual, 2)
IDOL_CREATE_ATTRIBUTE(Solution, IDOL_Solution_Dual, 0, Duals, void)
IDOL_CREATE_ATTRIBUTE(Solution, IDOL_Solution_Dual, 1, Farkas, void)

#endif //IDOL_ATTRIBUTES_MODEL_H
