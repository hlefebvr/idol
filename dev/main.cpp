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

#include <iostream>
#include <OsiCpxSolverInterface.hpp>
#include <OsiCbcSolverInterface.hpp>

void hello_world_osi_idol() {

    using namespace idol;

    Env env;
    Model model(env);
    model.use(Gurobi().with_logs(true));

    OsiIdolSolverInterface interface(model);
    std::unique_ptr<OsiSolverInterface> solver(interface.clone());
    // solver->messageHandler()->setLogLevel(0);

    /*
    *  optimal solution: x* = (1,1)
    *
    *  minimize -1 x0 - 1 x1
    *  s.t       1 x0 + 2 x1 <= 3
    *            2 x0 + 1 x1 <= 3
    *              x0        >= 0
    *              x1        >= 0
    */

    solver->addCol(CoinPackedVector(), 0., solver->getInfinity(), -1.);
    solver->addCol(CoinPackedVector(), 0., solver->getInfinity(), -1.);

    //-infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    solver->addRow(row1, -solver->getInfinity(), 3.);

    //-infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    solver->addRow(row2, -solver->getInfinity(), 3.);

    solver->initialSolve();

    solver->writeLp("model");
}

int main(int t_argc, const char** t_argv) {

    hello_world_osi_idol();

    return 0;
}
