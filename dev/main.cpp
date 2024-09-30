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

void solve_with_mibs(int t_argc, const char** t_argv) {

    std::unique_ptr<OsiSolverInterface> solver(new OsiClpSolverInterface());

    MibSModel model;
    model.setSolver(solver.get());

    // AUXILIARY DATA
    std::vector<int> upper_level_variables_indices = { 0,  };
    std::vector<int> lower_level_variables_indices = { 1,  };
    std::vector<int> upper_level_constraints_indices = { 0, 1,  };
    std::vector<int> lower_level_constraints_indices = { 2, 3,  };
    std::vector<double> lower_level_objective_coefficients = { 1,  };

    model.loadAuxiliaryData(
            (int) lower_level_variables_indices.size(),
            (int) lower_level_constraints_indices.size(),
            lower_level_variables_indices.data(),
            lower_level_constraints_indices.data(),
            1.,
            lower_level_objective_coefficients.data(),
            (int) upper_level_variables_indices.size(),
            (int) upper_level_constraints_indices.size(),
            upper_level_variables_indices.data(),
            upper_level_constraints_indices.data(),
            0,
            nullptr,
            0,
            nullptr,
            nullptr,
            nullptr
    );

    // PROBLEM DATA

    std::vector<double> variable_lower_bounds = { 0, 0,  };
    std::vector<double> variable_upper_bounds = { 10, 5,  };
    std::vector<char> variable_types = { 'I', 'I',  };
    std::vector<double> constraint_lower_bounds = { -1e+20, -1e+20, -1e+20, -1e+20,  };
    std::vector<double> constraint_upper_bounds = { 12, 20, 7, 16,  };
    std::vector<char> constraint_types = { 'L', 'L', 'L', 'L',  };
    std::vector<double> objective = { -1, -7,  };

    const bool is_column_oriented = true;

    CoinPackedMatrix matrix(is_column_oriented, 0, 0);

    if (is_column_oriented) {

        CoinPackedVector col1;
        col1.insert(0, -3);
        col1.insert(1, 1);
        col1.insert(2, 2);
        col1.insert(3, -2);
        matrix.appendCol(col1);

        CoinPackedVector col2;
        col2.insert(0, 2);
        col2.insert(1, 2);
        col2.insert(2, -1);
        col2.insert(3, 4);
        matrix.appendCol(col2);

    } else {

        CoinPackedVector row1;
        row1.insert(0, -3);
        row1.insert(1, 2);
        matrix.appendRow(row1);

        CoinPackedVector row2;
        row2.insert(0, 1);
        row2.insert(1, 2);
        matrix.appendRow(row2);

        CoinPackedVector row3;
        row3.insert(0, 2);
        row3.insert(1, -1);
        matrix.appendRow(row3);

        CoinPackedVector row4;
        row4.insert(0, -2);
        row4.insert(1, 4);
        matrix.appendRow(row4);

        matrix.reverseOrdering();

    }

    model.loadProblemData(
            matrix,
            variable_lower_bounds.data(),
            variable_upper_bounds.data(),
            objective.data(),
            constraint_lower_bounds.data(),
            constraint_upper_bounds.data(),
            variable_types.data(),
            1.,
            1e+20,
            constraint_types.data()
    );

    AlpsKnowledgeBrokerSerial broker(t_argc, (char**) t_argv, model, false);
    broker.search(&model);

    std::cout << "Done." << std::endl;

}

using namespace idol;

void solve_with_idol() {
    Env env;

    Model model(env);

    auto x = model.add_var(0, Inf, Integer, "x");
    auto y = model.add_var(0, Inf, Integer, "y");

    model.set_obj_expr(-x - 7 * y);
    const auto c1 = model.add_ctr(-3 * x + 2 * y <= 12);
    const auto c2 = model.add_ctr(x + 2 * y <= 20);
    const auto c3 = model.add_ctr(x <= 10);
    const auto c4 = model.add_ctr(2 * x - y <= 7);
    const auto c5 = model.add_ctr(-2 * x + 4 * y <= 16);
    const auto c6 = model.add_ctr(y <= 5);

    Bilevel::LowerLevelDescription decomposition(env);
    decomposition.set_follower_obj_expr(y);
    decomposition.make_follower_var(y);
    decomposition.make_follower_ctr(c4);
    decomposition.make_follower_ctr(c5);
    decomposition.make_follower_ctr(c6);

    // Use coin-or/MibS as external solver
    model.use(Bilevel::MibS(decomposition).with_logs(true));

    // Optimize and print solution
    model.optimize();
}

int main(int t_argc, const char** t_argv) {

    /*
     *  min  -x − 7y
         s.t.   -3x + 2y ≤ 12
                     x + 2y ≤ 20
                     x ≤ 10
                     y ∈ arg min {z : 2x - z ≤ 7,
                                      -2x + 4z ≤ 16,
                                      z ≤ 5}
     */

    solve_with_mibs(t_argc, t_argv);

    std::cout << "------------------------\n\n\n\n" << std::endl;

    solve_with_idol();

    return 0;
}
