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

#include <iostream>
#include <OsiCpxSolverInterface.hpp>
#include <OsiCbcSolverInterface.hpp>

void hello_world_osi() {

    OsiCpxSolverInterface interface;
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
}

using namespace idol;

void hello_world_osi_idol() {

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

    const auto instance = Problems::GAP::read_instance("/home/henri/Research/idol/examples/assignment.data.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    // Create optimization environment
    Env env;

    // Create model
    Model model(env);

    // Create decomposition annotation
    Annotation<Ctr> decomposition(env, "decomposition", MasterId);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        auto capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));

        capacity.set(decomposition, i); // Assign constraint to i-th subproblem
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    // Set optimizer
    model.use(BranchAndBound()
                      .with_node_optimizer(
                              DantzigWolfeDecomposition(decomposition)
                                      .with_master_optimizer(
                                              Osi::ContinuousRelaxation(OsiCpxSolverInterface())
                                                                .with_logs(false)
                                      )
                                      .with_default_sub_problem_spec(
                                              DantzigWolfe::SubProblem()
                                                      .add_optimizer(Osi(OsiCpxSolverInterface()).with_logs(false))
                                                      .with_column_pool_clean_up(1500, .75)
                                      )
                                      .with_logger(Logs::DantzigWolfe::Info().with_frequency_in_seconds(.0001).with_sub_problems(true))
                                      .with_logs(true)
                                      .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(.3))
                                      .with_infeasibility_strategy(DantzigWolfe::ArtificialCosts().with_max_updates_before_phase_I(0))
                                      .with_hard_branching(true)
                      )
                      .with_subtree_depth(0)
                      .with_branching_rule(MostInfeasible())
                      .with_node_selection_rule(WorstBound())
                      .add_callback(Heuristics::IntegerMaster().with_optimizer(Osi(OsiCpxSolverInterface()).with_logs(false)))
                      .with_logs(true)
    );

    // Solve
    model.optimize();

    // Print solution
    std::cout << save_primal(model) << std::endl;

    return 0;
}
