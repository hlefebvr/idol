#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/archive/column-generation/IntegerMaster.h"
#include "idol/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/archive/dantzig-wolfe/ArchivedDantzigWolfeDecomposition.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Wentges.h"

using namespace idol;

int main(int t_argc, char** t_argv) {


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
                                    .with_master_optimizer(Gurobi::ContinuousRelaxation())
                                    .with_default_sub_problem_spec(
                                            DantzigWolfe::SubProblem()
                                                          .add_optimizer(Gurobi())
                                                          .with_multiplicities(0, 1)
                                      )
                                    .with_infeasibility_strategy(DantzigWolfe::FarkasPricing())
                                    .with_hard_branching(false)
                                    .with_max_parallel_sub_problems(5)
                                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Wentges(.3))
                                    .with_log_level(Info, Yellow)
                      )
                      .with_subtree_depth(0)
                      .with_branching_rule(MostInfeasible())
                      .with_node_selection_rule(WorstBound())
                      .with_log_level(Info, Blue)
                      .with_log_frequency(1)
    );

    // Solve
    model.optimize();

    std::cout << save_primal(model) << std::endl;

    return 0;
}
