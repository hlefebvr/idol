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

    {

    int n_cols = 2;
    std::vector<double> objective{-1., -1.};
    std::vector<double> lower_bounds{0., 0.};
    std::vector<double> upper_bounds{solver->getInfinity(), solver->getInfinity()};

    int n_rows = 2;
    std::vector<double> row_lower_bounds = {-solver->getInfinity(), -solver->getInfinity()};
    std::vector<double> row_upper_bounds = {3., 3.};

    //Define the constraint matrix.
    CoinPackedMatrix matrix(false, 0, 0);
    matrix.setDimensions(0, n_cols);

    //-infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    matrix.appendRow(row1);

    //-infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    matrix.appendRow(row2);

    //load the problem to OSI
    solver->loadProblem(matrix,
                        lower_bounds.data(),
                        upper_bounds.data(),
                        objective.data(),
                        row_lower_bounds.data(),
                        row_upper_bounds.data());

    }

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
