//
// Created by henri on 13/02/23.
//

#ifndef IDOL_SOLUTION_TYPES_H
#define IDOL_SOLUTION_TYPES_H

#include <string>
#include <stdexcept>
#include <ostream>
#include <iomanip>
#include "idol/general/utils/exceptions/Exception.h"

namespace idol {

    enum SolutionStatus {
        Loaded = 0,
        Optimal = 1,
        Feasible = 2,
        Infeasible = 3,
        InfOrUnbnd = 4,
        Unbounded = 5,
        Fail = 6,
        SubOptimal = 7
    };

    enum SolutionReason {
        NotSpecified = 0,
        Proved = 1,
        TimeLimit = 2,
        IterLimit = 3,
        ObjLimit = 4,
        Numerical = 5,
        MemoryLimit = 6,
        Cycling = 7,
        SolutionLimit = 8
    };

    template<class T>
    bool is_in();

    template<typename T>
    bool is_in(T t_status, std::initializer_list<T> t_list) {
        for (auto status: t_list) {
            if (status == t_status) {
                return true;
            }
        }
        return false;
    }

    template<typename T>
    bool is_in(typename T::value_type t_status, const T &t_list) {
        for (auto status: t_list) {
            if (status == t_status) {
                return true;
            }
        }
        return false;
    }

    static SolutionStatus dual(SolutionStatus t_status) {
        switch (t_status) {
            case Loaded:
                return Loaded;
            case Optimal:
                return Optimal;
            case Feasible:
                return SubOptimal;
            case Infeasible:
                return Unbounded;
            case InfOrUnbnd:
                return InfOrUnbnd;
            case Unbounded:
                return Infeasible;
            case Fail:
                return Fail;
            case SubOptimal:
                return Feasible;
        }
        throw Exception("Unexpected status: " + std::to_string(t_status));
    }

    static std::ostream &operator<<(std::ostream &t_os, SolutionStatus t_status) {

        switch (t_status) {
            case Loaded:
                return t_os << "Loaded";
            case Optimal:
                return t_os << "Optimal";
            case Feasible:
                return t_os << "Feasible";
            case Infeasible:
                return t_os << "Infeasible";
            case InfOrUnbnd:
                return t_os << "InfOrUnbnd";
            case Unbounded:
                return t_os << "Unbounded";
            case Fail:
                return t_os << "Fail";
            case SubOptimal:
                return t_os << "SubOptimal";
            default:;
        }

        throw Exception("Unexpected status: " + std::to_string(t_status));
    }

    static std::ostream &operator<<(std::ostream &t_os, SolutionReason t_status) {

        switch (t_status) {
            case Proved:
                return t_os << "Proved";
            case NotSpecified:
                return t_os << "-";
            case TimeLimit:
                return t_os << "TimeLimit";
            case IterLimit:
                return t_os << "IterLimit";
            case ObjLimit:
                return t_os << "ObjLimit";
            case Numerical:
                return t_os << "Numerical";
            case MemoryLimit:
                return t_os << "MemoryLimit";
            case Cycling:
                return t_os << "Cycling";
            case SolutionLimit:
                return t_os << "SolutionLimit";
            default:;
        }

        throw Exception("Unexpected status: " + std::to_string(t_status));
    }

}

#endif //IDOL_SOLUTION_TYPES_H
