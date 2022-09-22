//
// Created by henri on 20/09/22.
//
#include <iostream>

#include "algorithms/logs/Log.h"

#include "solve_with_lpsolve.h"
#include "solve_with_branch_and_bound.h"
#include "solve_with_branch_and_price.h"

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    Instance instance;

    solve_with_lpsolve(instance);
    solve_with_branch_and_bound(instance);
    solve_with_branch_and_price(instance);

    return 0;
}

