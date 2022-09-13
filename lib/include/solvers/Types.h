//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLVERS_TYPES_H
#define OPTIMIZE_SOLVERS_TYPES_H

#include <string>
#include <stdexcept>
#include <ostream>

enum SolutionStatus {
    Unknown,
    Optimal,
    Feasible,
    Infeasible,
    FeasibleTimeLimit,
    InfeasibleTimeLimit,
    InfeasibleOrUnbounded,
    Unbounded,
    Error
};

enum AlgorithmForLP {
    Automatic,
    PrimalSimplex,
    DualSimplex,
    Barrier
};

static bool is_in(SolutionStatus t_status, std::initializer_list<SolutionStatus> t_list) {
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
        case FeasibleTimeLimit: return FeasibleTimeLimit;
        case InfeasibleTimeLimit: return InfeasibleTimeLimit;
        case InfeasibleOrUnbounded: return InfeasibleOrUnbounded;
        case Unbounded: return Infeasible;
        case Error: return Error;
    }
    throw std::runtime_error("Unexpected status: " + std::to_string(t_status));
}

static std::ostream &operator<<(std::ostream& t_os, SolutionStatus t_status) {
    switch (t_status) {
        case Unknown: return t_os << "Unknown";
        case Optimal: return t_os << "Optimal";
        case Feasible: return t_os << "Feasible";
        case Infeasible: return t_os << "Infeasible";
        case FeasibleTimeLimit: return t_os << "FeasibleTimeLimit";
        case InfeasibleTimeLimit: return t_os << "InfeasibleTimeLimit";
        case InfeasibleOrUnbounded: return t_os << "InfeasibleOrUnbounded";
        case Unbounded: return t_os << "Unbounded";
        case Error: return t_os << "Error";
        default: throw std::runtime_error("Unexpected status: " + std::to_string(t_status));
    }
    return t_os;
}

#endif //OPTIMIZE_SOLVERS_TYPES_H
