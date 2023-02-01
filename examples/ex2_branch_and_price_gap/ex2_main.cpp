//
// Created by henri on 20/09/22.
//
#include <iostream>

#include "backends/parameters/Logs.h"

#include "solve_with_external_solver.h"
#include "solve_with_branch_and_price.h"

int main() {

    using namespace Problems::GAP;

    auto instance = read_instance("demo.txt");

    solve_with_external_solver(instance);
    solve_with_branch_and_price(instance);

    return 0;
}

