#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"
#include "idol/modeling.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Osi/OsiIdolSolverInterface.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/optimizers/mixed-integer-programming/wrappers/Osi/Osi.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/optimizers/mixed-integer-programming/callbacks/cutting-planes/KnapsackCover.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/mixed-integer-programming/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/mixed-integer-programming/dantzig-wolfe/logs/Info.h"
#include "idol/optimizers/mixed-integer-programming/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/mixed-integer-programming/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/mixed-integer-programming/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/mixed-integer-programming/callbacks/heuristics/IntegerMaster.h"
#include "idol/optimizers/mixed-integer-programming/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Bilevel.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Dualize.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"

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

void test_mibs() {

    OsiSymSolverInterface solver;

    MibSModel mibs;
    mibs.setSolver(&solver);

    const double inf = solver.getInfinity();

    int xi_1 = 0;
    int xi_2 = 1;
    int y_1  = 2;
    int y_2  = 3;

    {

        int n_lower_level_variables = 2;
        int n_upper_level_variables = 2;
        int n_lower_level_constraints = 2;
        int n_upper_level_constraints = 1;
        auto* lower_level_variables_indices = new int[2]{ y_1, y_2 };
        auto* lower_level_constraints_indices = new int[2]{1, 2};
        auto* lower_level_objective_coefficients = new double[2]{2, 3};
        auto* upper_level_variables_indices = new int[2]{xi_1, xi_2};
        auto* upper_level_constraints_indices = new int[1]{0};

        mibs.loadAuxiliaryData(n_lower_level_variables,
                               n_lower_level_constraints,
                               lower_level_variables_indices,
                               lower_level_constraints_indices,
                               1.,
                               lower_level_objective_coefficients,
                               n_upper_level_variables,
                               n_upper_level_constraints,
                               upper_level_variables_indices,
                               upper_level_constraints_indices,
                               0,
                               nullptr,
                               0,
                               nullptr,
                               nullptr,
                               nullptr);

    }

    {

        CoinPackedMatrix matrix(false, 0, 4);

        {
            CoinPackedVector row;
            row.insert(xi_1, 1);
            row.insert(xi_2, 1);
            matrix.appendRow(row);
        }

        {
            CoinPackedVector row;
            row.insert(y_1, -2);
            row.insert(y_2, 1);
            row.insert(xi_1, 3);
            matrix.appendRow(row);
        }

        {
            CoinPackedVector row;
            row.insert(y_1, 3);
            row.insert(y_2, 3);
            row.insert(xi_1, -2);
            row.insert(xi_2, 3);
            matrix.appendRow(row);
        }

        auto* constraint_lower_bounds = new double[3]{-inf, 0, 1};
        auto* constraint_upper_bounds = new double[3]{1, inf, inf};
        auto* constraint_types = new char[3]{'L', 'G', 'G'};
        auto* variable_types = new char[4]{'B', 'B', 'B', 'B'};
        auto* variable_lower_bounds = new double[4]{0, 0, 0, 0};
        auto* variable_upper_bounds = new double[4]{1, 1, 1, 1};
        auto* objective = new double[4]{0, 0, -2, -3};

        std::cout << matrix.getNumCols() << ", " << matrix.getNumRows() << std::endl;
        std::cout << matrix.getMajorDim() << ", " << matrix.getMinorDim() << std::endl;

        mibs.loadProblemData(matrix,
                             variable_lower_bounds,
                             variable_upper_bounds,
                             objective,
                             constraint_lower_bounds,
                             constraint_upper_bounds,
                             variable_types,
                             1.,
                             inf,
                             constraint_types);

    }

    std::string arg = "mibs";
    int argc = 1;
    char** argv = new char* [1];
    argv[0] = arg.data();

    AlpsKnowledgeBrokerSerial broker(argc, argv, mibs);
    broker.search(&mibs);

}

void test_idol_mibs() {

    using namespace idol;

    Env env;

    Model model(env);

    auto xi = model.add_vars(Dim<1>(2), 0, 1, Binary, "xi");
    auto y = model.add_vars(Dim<1>(2), 0, 1, Binary, "y");

    auto c1 = model.add_ctr(xi[0] + xi[1] <= 1);
    auto c2 = model.add_ctr(-2 * y[0] + y[1] >= -3 * xi[0]);
    auto c3 = model.add_ctr(3 * y[0] + 3 * y[1] >= 1 + 2 * xi[0] - 3 * xi[1]);
    auto lower_level_objective = model.add_ctr(2 * y[0] + 3 * y[1] == 0);

    Annotation<Var> lower_level_var(env, "ll_vars", MasterId);
    Annotation<Ctr> lower_level_ctr(env, "ll_ctrs", MasterId);

    y[0].set(lower_level_var, 0);
    y[1].set(lower_level_var, 0);
    c2.set(lower_level_ctr, 0);
    c3.set(lower_level_ctr, 0);

    model.use(Bilevel::MibS(lower_level_var, lower_level_ctr, lower_level_objective));

    model.optimize();

}

using namespace idol;

int main(int t_argc, char** t_argv) {

    Env env;

    auto [model,
          var_annotation,
          ctr_annotation,
          lower_level_objective] = Bilevel::read_from_file<Gurobi>(env, "/home/henri/Research/bilevel-ccg/code/data/milp/BCPIns_8_7_1.txt_trad.txt_K5.aux");

    /*
    Model model(env);

    auto x = model.add_var(0, 10, Binary, "x");
    auto z = model.add_var(0, 5, Integer, "z");

    model.set_obj_expr(-x - 7 * z);
    auto c1 = model.add_ctr(-3 * x + 2 * z <= 12);
    auto c2 = model.add_ctr(x + 2 * z <= 20);
    auto c3 = model.add_ctr(z == 0);
    auto c4 = model.add_ctr(2 * x - z <= 7);
    auto c5 = model.add_ctr(-2 * x + 4 * z <= 16);

    // Annotate follower variables
    // * If not annotated, the default value is MasterId, i.e., leader variables and constraints
    // * Otherwise, it indicates the id of the follower (here, we have only one follower)
    Annotation<Var> var_annotation(env, "follower_variable", MasterId);
    Annotation<Ctr> ctr_annotation(env, "ctr_annotation", MasterId);

    z.set(var_annotation, 0); // "z" is a lower level variable
    c4.set(ctr_annotation, 0); // "c4" is a lower level constraint
    c5.set(ctr_annotation, 0); // "c5" is a lower level constraint

    const auto& lower_level_objective = c3;
    */

    model.use(
                Bilevel::ColumnAndConstraintGeneration(var_annotation,
                                                      ctr_annotation,
                                                      lower_level_objective)
                    .with_master_optimizer(Gurobi())
                    .with_lower_level_optimizer(Gurobi())
                    .with_logs(true)
            );

    model.optimize();

    /*
    // Set Optimizer
    auto ccg = Robust::ColumnAndConstraintGeneration(variable_level, constraint_level, uncertainty_set)
            .with_master_optimizer(Gurobi().with_logs(false))
            .with_separator(
                    Dualize()
                                .with_optimizer(Gurobi().with_logs(false))
            )
            .with_complete_recourse(false)
            .with_logs(true)
            .with_iteration_limit(5)
        ;
        */

    return 0;
}
