#include "modeling.h"
#include "optimizers/solvers/Optimizers_Gurobi.h"
#include "optimizers/Logger.h"
#include "optimizers/branch-and-bound/Optimizers_BranchAndBound.h"
#include "optimizers/solvers/Optimizers_Gurobi.h"
#include "optimizers/solvers/DefaultOptimizer.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "optimizers/solvers/Optimizers_Mosek.h"
#include "optimizers/solvers/Gurobi.h"
#include "optimizers/solvers/Gurobi.h"
#include "optimizers/column-generation/ColumnGeneration.h"
#include "optimizers/column-generation/Optimizers_ColumnGeneration.h"
#include "optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "optimizers/solvers/Mosek.h"
#include "optimizers/column-generation/IntegerMasterHeuristic.h"

int main(int t_argc, char** t_argv) {

    // Read instance
    const auto instance = Problems::GAP::read_instance("/home/henri/CLionProjects/optimize/tests/instances/generalized-assignment-problem/GAP_instance0.txt");
    //const auto instance = Problems::GAP::read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/n2/instance_n2_30__0.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Env env;

    // Make model
    Model model(env);

    // Create decomposition annotation
    Annotation<Ctr> std_decomposition(env, "std_decomposition", MasterId);
    Annotation<Ctr> decomposition(env, "decomposition", MasterId);
    Annotation<Ctr> decomposition2(env, "decomposition2", MasterId);

    // Create assignment variables (x_ij binaries)
    auto x = model.add_vars(Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        auto capacity = model.add_ctr(idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));

        capacity.set(std_decomposition, i);
        capacity.set(decomposition, i/2);
        capacity.set(decomposition2, i % 2);
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
    }

    // Set the objective function
    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    for (bool primal_heuristic : { true, false }) {

        for (bool branching_on_master: {true}) {

            for (bool farkas_pricing: {true, false}) {

                for (double smoothing: {0., .3, .5, .8}) {

                    model.use(

                            BranchAndBound<NodeInfo>()

                                    .with_node_solver(

                                            DantzigWolfeDecomposition(std_decomposition)

                                                    .with_master_solver(
                                                            Gurobi::ContinuousRelaxation()
                                                                //.with_infeasible_or_unbounded_info(true)
                                                    )

                                                    .with_pricing_solver(
                                                            Gurobi()
                                                    )

                                                    .with_log_level(Info, Magenta)

                                                    .with_farkas_pricing(farkas_pricing)

                                                    .with_dual_price_smoothing_stabilization(smoothing)

                                                    .with_branching_on_master(branching_on_master)

                                                    //.with_log_frequency(1)

                                                    //.with_iteration_count_limit(20)

                                    )

                                    .with_branching_rule(MostInfeasible())

                                    .with_node_selection_rule(BestBound())

                                    .conditional(primal_heuristic, [](auto &x) {
                                        x.with_callback(
                                                IntegerMasterHeuristic().with_solver(Gurobi().with_time_limit(20))
                                        );
                                    })

                                    .with_subtree_depth(1)

                                    .with_log_level(Info, Blue)
                    );

                    std::cout << "RUNNING: " << branching_on_master << ", " << farkas_pricing << ", " << smoothing
                              << std::endl;

                    model.optimize();

                    std::cout << "RESULT: " << branching_on_master << ", " << farkas_pricing << ", " << smoothing
                              << std::endl;

                    std::cout << model.get_status() << std::endl;
                    std::cout << model.get_reason() << std::endl;
                    std::cout << save_primal_values(model) << std::endl;

                }

            }

        }

    }

    return 0;
/*

    Env env;

    BlockModel<Var> model(env, 1);
    auto& subproblem = model.block(0);

    // Master variables
    Var theta(env, 0, Inf, Continuous, "theta");
    Var x(env, 0, Inf, Continuous, "x");

    model.add(theta);
    model.add(x);
    model.master().set_obj(2 * x + theta);

    // Subproblem variables
    auto lambda = Var::array(env, Dim<1>(2), 0, Inf, Continuous, "lambda");
    subproblem.model().add_array<Var, 1>(lambda);

    // Subproblem constraints
    Ctr c1(env, lambda[0] + 2 * lambda[1] <= 2.);
    subproblem.model().add(c1);

    Ctr c2(env, 2 * lambda[0] - lambda[1] <= 3.);
    subproblem.model().add(c2);

    // Generation pattern
    subproblem.generation_pattern() = Row(theta + (!lambda[0] + 3. * !lambda[1]) * x,  3. * !lambda[0] + 4. * !lambda[1]);

    // Set epigraph
    subproblem.set_aggregator(theta);

    subproblem.model().set_obj_sense(Maximize);

    model.build_opposite_axis();

    auto& rg = Idol::set_optimizer<RowGeneration>(model);

    rg.set_master_backend<Gurobi>();
    rg.set_subproblem_backend<Gurobi>(0);


    model.optimize();
*/
    return 0;
}
