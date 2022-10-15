//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_TYPES_H
#define OPTIMIZE_TYPES_H

enum VarType {
    Continuous,
    Integer,
    Binary
};

enum CtrType {
    LessOrEqual,
    GreaterOrEqual,
    Equal
};

enum ObjectStatus {
    InModel,
    Removed
};

#endif //OPTIMIZE_TYPES_H
