#include <MibSSolution.hpp>
#include <OsiSymSolverInterface.hpp>

#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"

int main(int t_argc, char** t_argv) {

    auto solver = std::make_unique<OsiSymSolverInterface>();
    solver->messageHandler()->setLogLevel(0);

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

    // Check the solution
    if ( solver->isProvenOptimal() ) {
        std::cout << "Found optimal solution!" << std::endl;
        std::cout << "Objective value is " << solver->getObjValue() << std::endl;

        int n = solver->getNumCols();
        const double* solution = solver->getColSolution();

        // We can then print the solution or could examine it.
        for( int i = 0; i < n; ++i )
            std::cout << solver->getColName(i) << " = " << solution[i] << std::endl;

    } else {
        std::cout << "Didn't find optimal solution." << std::endl;
        // Could then check other status functions.
    }

    return 0;
}
