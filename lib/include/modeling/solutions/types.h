//
// Created by henri on 13/02/23.
//

#ifndef IDOL_SOLUTION_TYPES_H
#define IDOL_SOLUTION_TYPES_H

#include <string>
#include <stdexcept>
#include <ostream>
#include <iomanip>
#include "../../errors/Exception.h"

enum SolutionStatus {
    Loaded,
    Optimal,
    Feasible,
    Infeasible,
    InfOrUnbnd,
    Unbounded,
    Fail,
    SubOptimal
};

enum SolutionReason {
    NotSpecified,
    Proved,
    TimeLimit,
    IterLimit,
    ObjLimit,
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
        case Loaded: return Loaded;
        case Optimal: return Optimal;
        case Feasible: return SubOptimal;
        case Infeasible: return Unbounded;
        case InfOrUnbnd: return InfOrUnbnd;
        case Unbounded: return Infeasible;
        case Fail: return Fail;
        case SubOptimal: return Feasible;
    }
    throw Exception("Unexpected status: " + std::to_string(t_status));
}

static std::ostream &operator<<(std::ostream& t_os, SolutionStatus t_status) {

    switch (t_status) {
        case Loaded: return t_os << std::setw(10) << "Loaded";
        case Optimal: return t_os << std::setw(10) << "Optimal";
        case Feasible: return t_os << std::setw(10) << "Feasible";
        case Infeasible: return t_os << std::setw(10) << "Infeasible";
        case InfOrUnbnd: return t_os << std::setw(10) << "InfOrUnbnd";
        case Unbounded: return t_os << std::setw(10) << "Unbounded";
        case Fail: return t_os << std::setw(10) << "Fail";
        case SubOptimal: return t_os << std::setw(10) << "SubOptimal";
        default:;
    }

    throw Exception("Unexpected status: " + std::to_string(t_status));
}

static std::ostream &operator<<(std::ostream& t_os, SolutionReason t_status) {

    switch (t_status) {
        case Proved: return t_os << std::setw(9) << "Proved";
        case NotSpecified: return t_os << std::setw(9) << "-";
        case TimeLimit: return t_os << std::setw(9) << "TimeLimit";
        case IterLimit: return t_os << std::setw(9) << "IterLimit";
        case ObjLimit: return t_os << std::setw(9) << "ObjLimit";
        case Numerical: return t_os << std::setw(9) << "Numerical";
        default:;
    }

    throw Exception("Unexpected status: " + std::to_string(t_status));
}

#endif //IDOL_SOLUTION_TYPES_H
