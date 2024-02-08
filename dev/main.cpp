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

int main(int t_argc, char** t_argv) {

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
        model.add_ctr(idol_Sum(i, Range(n_facilities), y[i][j]) == 1);
    }

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            model.add_ctr(y[i][j] <= (1 - !xi[i]) * x[i]);
        }
    }

    model.set_obj_expr(idol_Sum(i, Range(n_facilities),
                                instance.fixed_cost(i) * x[i]
                                + idol_Sum(j, Range(n_customers),
                                           instance.per_unit_transportation_cost(i, j) *
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

    std::cout << save_primal(model) << std::endl;

    return 0;
}
