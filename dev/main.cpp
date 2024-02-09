#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"
#include "idol/modeling.h"
#include "idol/optimizers/wrappers/MibS/MibS.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/wrappers/Osi/OsiIdolSolverInterface.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/optimizers/wrappers/Osi/Osi.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/optimizers/callbacks/cutting-planes/KnapsackCover.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/logs/Info.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/callbacks/heuristics/IntegerMaster.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/column-and-constraint-generation/separators/MaxMinBiLevel.h"
#include "idol/optimizers/wrappers/MibS/MibS.h"

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

    model.use(BiLevel::MibS(lower_level_var, lower_level_ctr, lower_level_objective));

    model.optimize();

}

int main(int t_argc, char** t_argv) {

    //test_mibs();
    //test_idol_mibs();

    // hello_world_osi_idol();

    using namespace idol;
    using namespace idol::BiLevel;
    using namespace idol::ColumnAndConstraintGenerationSeparators;

    Env env;

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/Research/idol/examples/facility.data.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();
    const double Gamma = 2;

    // Make uncertainty set
    Model uncertainty_set(env);

    auto xi = uncertainty_set.add_vars(Dim<1>(n_facilities), 0, 1, Binary, "xi");
    uncertainty_set.add_ctr(idol_Sum(i, Range(n_facilities), xi[i]) <= Gamma);

    // Make model
    Model model(env);

    auto x = model.add_vars(Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = model.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Binary, "y");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) <= 1);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            model.add_ctr(y[i][j] <= (1 - !xi[i]) * x[i]);
        }
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           -instance.per_unit_transportation_cost(i, j) *
                                           instance.demand(j) *
                                           y[i][j]
                                )
                       )
    );

    // Annotations
    Annotation<Var> lower_level_variables(env, "lower_level_variable", MasterId);
    Annotation<Ctr> lower_level_constraints(env, "lower_level_constraints", MasterId);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0; j < n_customers; ++j) {
            y[i][j].set(lower_level_variables, 0);
        }
    }

    for (auto& ctr : model.ctrs()) {
        ctr.set(lower_level_constraints, 0);
    }

    // Set Optimizer

    auto ccg = Robust::ColumnAndConstraintGeneration(lower_level_variables,lower_level_constraints, uncertainty_set)
            .with_master_optimizer(Gurobi())
            .with_separator(MaxMinBiLevel())
            .with_logs(true)
        ;

    model.use(ccg);

    model.optimize();

    const auto status = model.get_status();

    if (status == Optimal) {
        std::cout << save_primal(model) << std::endl;
    } else {
        std::cout << "CCG ended with status " << status << std::endl;
    }

    return 0;
}
