#include <iostream>
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"

using namespace idol;

int main(int t_argc, const char** t_argv) {

    // Store the initial sudoku grid
    const Vector<char, 2> initial_grid = {
            { 5, 3, 0, 0, 7, 0, 0, 0, 0 },
            { 6, 0, 0, 1, 9, 5, 0, 0, 0 },
            { 0, 9, 8, 0, 0, 0, 0, 6, 0 },
            { 8, 0, 0, 0, 6, 0, 0, 0, 3 },
            { 4, 0, 0, 8, 0, 3, 0, 0, 1 },
            { 7, 0, 0, 0, 2, 0, 0, 0, 6 },
            { 0, 6, 0, 0, 0, 0, 2, 8, 0 },
            { 0, 0, 0, 4, 1, 9, 0, 0, 5 },
            { 0, 0, 0, 0, 8, 0, 0, 7, 9 }
    };

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Add variables
    const auto x = model.add_vars(Dim<3>(9,9,9), 0, 1, Binary, 0, "x");

    // Add constraints

    // Each cell contains exactly one number:
    for (unsigned int i = 0 ; i < 9 ; ++i) {
        for (unsigned int j = 0; j < 9; ++j) {
            model.add_ctr(idol_Sum(k, Range(9), x[i][j][k]) == 1);
        }
    }

    // Each number appears exactly once in each row
    for (unsigned int i = 0 ; i < 9 ; ++i) {
        for (unsigned int k = 0 ; k < 9 ; ++k) {
            model.add_ctr(idol_Sum(j, Range(9), x[i][j][k]) == 1);
        }
    }

    // Each number appears exactly once in each column
    for (unsigned int j = 0 ; j < 9 ; ++j) {
        for (unsigned int k = 0 ; k < 9 ; ++k) {
            model.add_ctr(idol_Sum(i, Range(9), x[i][j][k]) == 1);
        }
    }

    // Each number appears exactly once in each 3×3 box
    for (unsigned int k = 0 ; k < 9 ; ++k) {
        for (unsigned int p = 1 ; p <= 3 ; ++p) {
            for (unsigned int q = 1 ; q <= 3 ; ++q) {
                auto c = model.add_ctr(
                        idol_Sum(i, Range(3 * p - 3, 3 * p),
                                    idol_Sum(j, Range(3 * q - 3, 3 * q),
                                        x[i][j][k]
                                    )) == 1
                              );
            }
        }
    }

    // Add initial grid constraint
    for (unsigned int i = 0 ; i < 9 ; ++i) {
        for (unsigned int j = 0 ; j < 9 ; ++j) {
            const char k = initial_grid[i][j];
            if (k != 0) {
                model.set_var_lb(x[i][j][k-1], 1); // Fixes x_ijk to 1
            }
        }
    }

    // Use Gurobi and optimize
    model.use(Gurobi());
    model.optimize();

    std::cout << "Solution status is " << model.get_status() << std::endl;

    if (model.get_status() == Optimal) {
        std::cout << "Solution found!" << std::endl;

        // Go over each row
        for (unsigned int i = 0 ; i < 9 ; ++i) {

            // Go over each column
            for (unsigned int j = 0 ; j < 9 ; ++j) {

                // Go over each number
                for (unsigned int k = 0 ; k < 9 ; ++k) {

                    const double x_val = model.get_var_primal(x[i][j][k]);
                    if (equals(x_val, 1, 1e-4)) { // Compares the value of x_ijk to 1 with tolerance 1e-4
                        std::cout << k+1 << ' ';
                        break;
                    }

                }

            }

            std::cout << '\n';
        }
        std::cout << std::endl;

    } else {
        std::cout << "No solution exists." << std::endl;
    }

    return 0;
}
