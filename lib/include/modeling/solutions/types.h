//
// Created by henri on 13/02/23.
//

#ifndef IDOL_SOLUTION_TYPES_H
#define IDOL_SOLUTION_TYPES_H

#include <string>
#include <stdexcept>
#include <ostream>
#include "../../errors/Exception.h"

enum SolutionStatus {
    Unknown,
    Optimal,
    Feasible,
    Infeasible,
    InfeasibleOrUnbounded,
    Unbounded,
    Fail
};

enum SolutionReason {
    NotSpecified,
    Proved,
    TimeLimit,
    IterationCount,
    CutOff,
    UserObjLimit,
    Numerical
};

template<class T> bool is_in();

template<typename T>
bool is_in(T t_status, std::initializer_list<T> t_list) {
    for (auto status : t_list) {
        if (status == t_status) {
            return true;
        }
    }
    return false;
}

template<typename T>
bool is_in(typename T::value_type t_status, const T& t_list) {
    for (auto status : t_list) {
        if (status == t_status) {
            return true;
        }
    }
    return false;
}

static SolutionStatus dual(SolutionStatus t_status) {
    switch (t_status) {
        case Unknown: return Unknown;
        case Optimal: return Optimal;
        case Feasible: return Feasible;
        case Infeasible: return Unbounded;
        case InfeasibleOrUnbounded: return InfeasibleOrUnbounded;
        case Unbounded: return Infeasible;
        case Fail: return Fail;
    }
    throw Exception("Unexpected status: " + std::to_string(t_status));
}

static std::ostream &operator<<(std::ostream& t_os, SolutionStatus t_status) {
    switch (t_status) {
        case Unknown: return t_os << "Unknown";
        case Optimal: return t_os << "Optimal";
        case Feasible: return t_os << "Feasible";
        case Infeasible: return t_os << "Infeasible";
        case InfeasibleOrUnbounded: return t_os << "InfeasibleOrUnbounded";
        case Unbounded: return t_os << "Unbounded";
        case Fail: return t_os << "Fail";
        default: throw Exception("Unexpected status: " + std::to_string(t_status));
    }
    return t_os;
}

static std::ostream &operator<<(std::ostream& t_os, SolutionReason t_status) {
    switch (t_status) {
        case Proved: return t_os << "Proved";
        case NotSpecified: return t_os << "NotSpecified";
        case TimeLimit: return t_os << "TimeLimit";
        case IterationCount: return t_os << "IterationCount";
        case CutOff: return t_os << "CutOff";
        case UserObjLimit: return t_os << "UserObjLimit";
        case Numerical: return t_os << "Numerical";
        default: throw Exception("Unexpected status: " + std::to_string(t_status));
    }
    return t_os;
}

#endif //IDOL_SOLUTION_TYPES_H
