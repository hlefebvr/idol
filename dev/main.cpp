#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"
#include "idol/modeling.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Osi/OsiIdolSolverInterface.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/cutting-planes/KnapsackCover.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/logs/Info.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"

#include <iostream>
#include <OsiCpxSolverInterface.hpp>
#include <OsiCbcSolverInterface.hpp>
#include <CglKnapsackCover.hpp>
#include <CglPreProcess.hpp>

int main(int t_argc, const char** t_argv) {

    using namespace idol;

    const auto instance = Problems::KP::read_instance("/home/henri/Research/idol/examples/mixed-integer-optimization/knapsack.data.txt");

    const auto n_items = instance.n_items();

    Env env;
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_items), 0, 1, Binary, "x");

    model.add_ctr(idol_Sum(j, Range(n_items), instance.weight(j) * x[j]) <= instance.capacity());

    model.set_obj_expr(idol_Sum(j, Range(n_items), -instance.profit(j) * x[j]));

    model.use(Gurobi::ContinuousRelaxation());
    auto solver = std::make_unique<OsiIdolSolverInterface>(model);

    OsiCuts cuts;

    std::cout << model << std::endl;

    CglKnapsackCover knapsack_cover;

    for (unsigned int i = 0 ; i < 10 ; ++i) {
        std::cout << model << std::endl;
        solver->resolve();
        knapsack_cover.generateCuts(*solver, cuts);
        solver->applyCuts(cuts);
    }

    std::cout << save_primal(model) << std::endl;

    return 0;
}
