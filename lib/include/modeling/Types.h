//
// Created by henri on 07/09/22.
//

#ifndef OPTIMIZE_TYPES_H
#define OPTIMIZE_TYPES_H

#include "../errors/Exception.h"

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

enum ObjSense {
    Minimize,
    Maximize
};

static std::ostream &operator<<(std::ostream& t_os, VarType t_type) {
    switch (t_type) {
        case Continuous: return t_os << "Continuous";
        case Integer: return t_os << "Integer";
        case Binary: return t_os << "Binary";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

static std::ostream &operator<<(std::ostream& t_os, CtrType t_type) {
    switch (t_type) {
        case LessOrEqual: return t_os << "<=";
        case GreaterOrEqual: return t_os << ">=";
        case Equal: return t_os << "=";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

static std::ostream &operator<<(std::ostream& t_os, ObjectStatus t_type) {
    switch (t_type) {
        case InModel: return t_os << "InModel";
        case Removed: return t_os << "Removed";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

static std::ostream &operator<<(std::ostream& t_os, ObjSense t_type) {
    switch (t_type) {
        case Minimize: return t_os << "Minimize";
        case Maximize: return t_os << "Maximize";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

#endif //OPTIMIZE_TYPES_H
