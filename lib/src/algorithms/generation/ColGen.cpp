//
// Created by henri on 13/12/22.
//
#include "algorithms/generation/ColGen.h"

ColGen::ColGen(Algorithm &t_rmp_solution_strategy) : Generation(t_rmp_solution_strategy) {

}

void ColGen::analyze_master_problem_solution() {

    auto status = rmp_solution_strategy().status();

    if (status == Optimal || status == Infeasible) {
        return;
    }

    if (status == Unbounded) {

        set_status(Unbounded);
        set_reason(Proved);

        idol_Log(Trace, "column-generation", "Terminate. Unbounded master problem.");

    } else {

        set_status(Fail);
        set_reason(NotSpecified);

        idol_Log(Trace, "column-generation", "Terminate. Master problem could not be solved to optimality.");

    }

    terminate();

}

void ColGen::solve_subproblems() {
    Generation::solve_subproblems();
}

void ColGen::analyze_subproblems_solution() {

}

