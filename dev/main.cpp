#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"

#include <iostream>
#include <OsiCpxSolverInterface.hpp>

int main(int t_argc, char** t_argv) {

    std::unique_ptr<OsiSolverInterface> solver(new OsiCpxSolverInterface());
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

    MibSModel model;
    model.setSolver(solver.get());

    std::vector<int> indices {0, 1 };
    std::vector<double> lower_objective { -1., -1. };

    model.loadAuxiliaryData(
            2,
            2,
            indices.data(),
            indices.data(),
            1.,
            lower_objective.data(),
            0,
            0,
            nullptr,
            nullptr,
            0,
            nullptr,
            0,
            nullptr,
            nullptr,
            nullptr
            );

    int argc = 1;
    std::string arg = "mibs";
    char** argv = new char* [1];
    argv[0] = arg.data();

    std::cout << argv[0] << std::endl;

    AlpsKnowledgeBrokerSerial broker(argc, argv, model);

    broker.search(&model);

    return 0;
}
