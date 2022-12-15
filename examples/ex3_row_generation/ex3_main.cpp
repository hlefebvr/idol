//
// Created by henri on 18/10/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms.h"
#include "problems/KP/KP_Instance.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"
#include "algorithms/row-generation/RowGeneration.h"
#include "algorithms/callbacks/Callbacks_RowGeneration.h"

int main() {

    Model sp;
    auto w_1 = sp.add_var(0., Inf, Continuous, 0., "w_1");
    auto w_2 = sp.add_var(0., Inf, Continuous, 0., "w_2");
    sp.add_ctr(w_1 + 2 * w_2 <= 2.);
    sp.add_ctr(2 * w_1 + -1 * w_2 <= 3.);

    Model rmp;
    auto z = rmp.add_var(0., Inf, Integer, 1., "z");
    auto y = rmp.add_var(0., Inf, Integer, 2., "y");
    auto ctr = rmp.add_ctr(z >= (-!w_1 - 3. * !w_2) * y + 3. * !w_1 + 4. * !w_2);

    /*
    Solvers::Gurobi gurobi(rmp);
    gurobi.add_callback<Callbacks::RowGeneration>(NewIncumbentFound)
            .add_subproblem(ctr)
            .set_solution_strategy<Solvers::Gurobi>(sp);
    gurobi.solve();
     */

    Decomposition decomposition;
    decomposition.set_rmp_solution_strategy<Solvers::Gurobi>(rmp);
    auto& cut_generation = decomposition.add_generation_strategy<RowGeneration>();
    auto &subproblem = cut_generation.add_subproblem(ctr);
    subproblem.set_solution_strategy<Solvers::Gurobi>(sp);
    decomposition.solve();

    return 0;
}

