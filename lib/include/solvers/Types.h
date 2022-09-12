//
// Created by henri on 12/09/22.
//

#ifndef OPTIMIZE_SOLVERS_TYPES_H
#define OPTIMIZE_SOLVERS_TYPES_H

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

static bool is_feasible(SolutionStatus t_status) {
    return t_status == Optimal || t_status == Feasible || t_status == FeasibleTimeLimit;
}

static std::ostream &operator<<(std::ostream& t_os, SolutionStatus t_status) {
    switch (t_status) {
        case Unknown: return t_os << "Unknown";
        case Optimal: return t_os << "Optimal";
        case Feasible: return t_os << "Feasible";
        case Infeasible: return t_os << "Infeasible";
        case FeasibleTimeLimit: t_os << "FeasibleTimeLimit";
        case InfeasibleTimeLimit: t_os << "InfeasibleTimeLimit";
        case InfeasibleOrUnbounded: t_os << "InfeasibleOrUnbounded";
        case Unbounded: t_os << "Unbounded";
        case Error: t_os << "Error";
        default: throw std::runtime_error("Unexpected status: " + std::to_string(t_status));
    }
    return t_os;
}

#endif //OPTIMIZE_SOLVERS_TYPES_H
