#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"
#include "idol/modeling.h"
#include "idol/optimizers/wrappers/MibS/impl_MibS.h"

#include <iostream>
#include <OsiCpxSolverInterface.hpp>

void hello_world_osi() {

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

    MibSModel model;
    model.setSolver(solver.get());

    std::vector<int> indices {0, 1 };
    std::vector<double> lower_objective { -1., -1. };

    solver->initialSolve();
}

void hello_world_mibs() {

    std::unique_ptr<OsiSolverInterface> solver(new OsiCpxSolverInterface());

    MibSModel model;
    model.setSolver(solver.get());

    /**
     * min  -x − 7y
     * s.t. −3x + 2y ≤ 12
             x + 2y ≤ 20
             x ≤ 10
             y ∈ arg min {z : 2x - z ≤ 7,
                              -2x + 4z ≤ 16,
                              z ≤ 5}
     */

    std::vector<int> upper_level_variables_indices = { 0,  };
    std::vector<int> lower_level_variables_indices = { 1,  };
    std::vector<int> upper_level_constraints_indices = { 0, 1,  };
    std::vector<int> lower_level_constraints_indices = { 2, 3,  };
    std::vector<double> lower_level_objective_coefficients = { 1  };

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

    std::vector<double> variable_lower_bounds = { 0, 0,  };
    std::vector<double> variable_upper_bounds = { 10, 5,  };
    std::vector<char> variable_types = { 'I', 'I',  };
    std::vector<double> constraint_lower_bounds = { -1e+20, -1e+20, -1e+20, -1e+20,  };
    std::vector<double> constraint_upper_bounds = { 12, 20, 7, 16,  };
    std::vector<char> constraint_types = { 'L', 'L', 'L', 'L',  };
    std::vector<double> objective = { -1, -7,  };

    CoinPackedMatrix matrix(false, 0, 2);

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

    matrix.setMinorDim(4);

    std::cout << "Here: " << matrix.getMinorDim() << std::endl;

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

    std::cout << "SUCCESS!" << std::endl;
}

using namespace idol;

int main(int t_argc, char** t_argv) {

    Env env;

    Annotation<Var> follower_variables(env, "follower_variable", MasterId);
    Annotation<Ctr> follower_constraints(env, "follower_constraints", MasterId);

    /**
     * min  -x − 7y
     * s.t. −3x + 2y ≤ 12
             x + 2y ≤ 20
             x ≤ 10
             y ∈ arg min {z : 2x - z ≤ 7,
                              -2x + 4z ≤ 16,
                              z ≤ 5}
     */

    Model model(env);

    auto x = model.add_var(0, 10, Integer, "x");
    auto y = model.add_var(0, 5, Integer, "y");

    model.set_obj_expr(-x - 7 * y);
    model.add_ctr(-3 * x + 2 * y <= 12);
    model.add_ctr(x + 2 * y <= 20);
    auto follower_objective = model.add_ctr(y == 0);
    auto follower_c1 = model.add_ctr(2 * x - y <= 7);
    auto follower_c2 = model.add_ctr(-2 * x + 4 * y <= 16);

    y.set(follower_variables, 0);
    follower_c1.set(follower_constraints, 0);
    follower_c2.set(follower_constraints, 0);

    impl::MibS impl(model, follower_variables, follower_constraints, follower_objective);

    impl.solve();

    std::cout << "objective value = " << impl.get_objective_value() << std::endl;
    std::cout << "status = " << impl.get_status() << std::endl;
    std::cout << "reason = " << impl.get_reason() << std::endl;
    std::cout << "x = " << impl.get_var_primal(x) << std::endl;
    std::cout << "y = " << impl.get_var_primal(y) << std::endl;


    return 0;
}
