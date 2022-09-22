//
// Created by henri on 20/09/22.
//
#include <iostream>

#include "algorithms/logs/Log.h"

#include "solve_with_external_solver.h"
#include "solve_with_branch_and_bound.h"
#include "solve_with_branch_and_price.h"

#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_0.h"
#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_1.h"
#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_2.h"
#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_3.h"
#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_4.h"
#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_5.h"
#include "../../tests/instances/generalized-assignment-problem/InstanceGAP_6.h"

int main() {

    Log::set_level(Info);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    InstanceGAP<6> instance;

    solve_with_external_solver(instance);
    //solve_with_branch_and_bound(instance);
    solve_with_branch_and_price(instance);

    return 0;
}

