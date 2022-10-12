//
// Created by henri on 20/09/22.
//
#include <iostream>

#include "algorithms/logs/Log.h"

#include "solve_with_external_solver.h"
#include "solve_with_branch_and_price.h"

int main() {

    Log::set_level(Debug);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    using namespace ProblemSpecific::GAP;

    auto instance = read_instance("demo.txt");

    solve_with_external_solver(instance);
    solve_with_branch_and_price(instance);

    return 0;
}

