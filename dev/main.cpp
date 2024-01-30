#include <MibSSolution.hpp>
#include <OsiSymSolverInterface.hpp>

#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"

int main(int t_argc, char** t_argv) {

    /** Set up lp solver **/
    auto solver = std::make_unique<OsiSymSolverInterface>();
    //solver->messageHandler()->setLogLevel(0);

    /*
    * This section adapted from Matt Galati's example
    * on the COIN-OR Tutorial website.
    *
    * Problem from Bertsimas, Tsitsiklis page 21
    *
    *  optimal solution: x* = (1,1)
    *
    *  minimize -1 x0 - 1 x1
    *  s.t       1 x0 + 2 x1 <= 3
    *            2 x0 + 1 x1 <= 3
    *              x0        >= 0
    *              x1        >= 0
    */

    int n_cols = 2;
    auto *objective    = new double[n_cols];//the objective coefficients
    auto *col_lb       = new double[n_cols];//the column lower bounds
    auto *col_ub       = new double[n_cols];//the column upper bounds

    //Define the objective coefficients.
    //minimize -1 x0 - 1 x1
    objective[0] = -1.0;
    objective[1] = -1.0;

    //Define the variable lower/upper bounds.
    // x0 >= 0   =>  0 <= x0 <= infinity
    // x1 >= 0   =>  0 <= x1 <= infinity
    col_lb[0] = 0.0;
    col_lb[1] = 0.0;
    col_ub[0] = solver->getInfinity();
    col_ub[1] = solver->getInfinity();

    int n_rows = 2;
    auto *row_lb = new double[n_rows]; //the row lower bounds
    auto *row_ub = new double[n_rows]; //the row upper bounds

    //Define the constraint matrix.
    auto *matrix =  new CoinPackedMatrix(false,0,0);
    matrix->setDimensions(0, n_cols);

    //1 x0 + 2 x1 <= 3  =>  -infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    row_lb[0] = -1.0 * solver->getInfinity();
    row_ub[0] = 3.0;
    matrix->appendRow(row1);

    //2 x0 + 1 x1 <= 3  =>  -infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    row_lb[1] = -1.0 * solver->getInfinity();
    row_ub[1] = 3.0;
    matrix->appendRow(row2);

    //load the problem to OSI
    solver->loadProblem(*matrix, col_lb, col_ub, objective, row_lb, row_ub);

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

    delete[] row_ub;
    delete[] row_lb;
    delete[] objective;
    delete[] col_ub;
    delete[] col_lb;

    return 0;
}
