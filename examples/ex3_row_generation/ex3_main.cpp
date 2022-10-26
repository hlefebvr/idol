//
// Created by henri on 18/10/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms.h"
#include "problems/kp/KP_Instance.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"
#include "algorithms/row-generation/RowGeneration.h"

int main() {

    Log::set_level(Trace);


    Model sp;
    auto w_1 = sp.add_variable(0., Inf, Continuous, 0., "w_1");
    auto w_2 = sp.add_variable(0., Inf, Continuous, 0., "w_2");
    sp.add_constraint(     w_1 +  2 * w_2 <= 2.);
    sp.add_constraint( 2 * w_1 + -1 * w_2 <= 3.);

    Model rmp;
    auto z = rmp.add_variable(0., Inf, Continuous, 1., "z");
    auto y = rmp.add_variable(0., Inf, Continuous, 2., "y");
    auto ctr = rmp.add_constraint( z >= (-!w_1 - 3. * !w_2) * y + 3. * !w_1 + 4. * !w_2 );

    Decomposition decomposition;
    decomposition.set_rmp_solution_strategy<Solvers::Gurobi>(rmp);
    auto& cut_generation = decomposition.add_generation_strategy<RowGeneration>();
    auto &subproblem = cut_generation.add_subproblem(ctr);
    subproblem.set_solution_strategy<Solvers::Gurobi>(sp);

    decomposition.solve();

    return 0;
}

