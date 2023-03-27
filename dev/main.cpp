#include "modeling.h"
#include "modeling/models/BlockModel.h"
#include "optimizers/row-generation/RowGeneration.h"
#include "optimizers/solvers/Gurobi.h"
#include "optimizers/parameters/Logs.h"
#include "optimizers/branch-and-bound/BranchAndBound.h"
#include "optimizers/solvers/GLPK.h"
#include "optimizers/solvers/DefaultOptimizer.h"
#include "optimizers/branch-and-bound/BranchAndBoundOptimizer.h"
#include "optimizers/branch-and-bound/relaxations/impls/ContinuousRelaxation.h"
#include "optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "optimizers/column-generation/ColumnGenerationOptimizer.h"
#include "optimizers/branch-and-bound/relaxations/impls/DantzigWolfeRelaxation.h"
#include "optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "optimizers/solvers/Mosek.h"
#include "optimizers/solvers/GurobiOptimizer.h"
#include "optimizers/solvers/GLPKOptimizer.h"
#include "optimizers/column-generation-v2/ColumnGenerationOptimizerV2.h"
#include "optimizers/column-generation-v2/ColumnGenerationV2.h"
#include "optimizers/dantzig-wolfe/DantzigWolfeOptimizer.h"

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBoundOptimizer<NodeInfo>>(Trace);
    Logs::set_color<BranchAndBoundOptimizer<NodeInfo>>(Blue);

    Logs::set_level<Backends::BranchAndBound<NodeInfo>>(Trace);
    Logs::set_color<Backends::BranchAndBound<NodeInfo>>(Blue);

    Logs::set_level<ColumnGenerationOptimizer>(Trace);
    Logs::set_color<ColumnGenerationOptimizer>(Yellow);

    Logs::set_level<Backends::ColumnGenerationV2>(Trace);
    Logs::set_color<Backends::ColumnGenerationV2>(Yellow);

    // Read instance
    const auto instance = Problems::GAP::read_instance("/home/henri/CLionProjects/optimize/tests/instances/generalized-assignment-problem/GAP_instance2.txt");

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
    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Add variables to the modelgeneration_pattern
    model.add_array<Var, 2>(x);

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        model.add(capacity);

        capacity.set(std_decomposition, i);
        capacity.set(decomposition, i/2);
        capacity.set(decomposition2, i % 2);
    }

    // Create assignment constraints
    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    // Set the objective function
    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    /*
    model.use(BranchAndBoundOptimizer<NodeInfo>(
                DantzigWolfeOptimizer(
                    decomposition,
                    GurobiOptimizer(),
                    BranchAndBoundOptimizer<NodeInfo>(
                        DantzigWolfeOptimizer(
                                decomposition2,
                                GurobiOptimizer(),
                                BranchAndBoundOptimizer<NodeInfo>(
                                    GLPKOptimizer(),
                                    ContinuousRelaxation(),
                                    MostInfeasible(),
                                    WorstBound()
                                )
                        ),
                        ContinuousRelaxation(),
                        MostInfeasible(),
                        DepthFirst()
                    )
                ),
                ContinuousRelaxation(),
                MostInfeasible(),
                BestBound()
            ));
    */

    //model.set(Attr::Var::Ub, x[0][2], 0);

    for (bool branching_on_master : { true, false }) {

        for (bool farkas_pricing : { false, true }) {

            for (double smoothing : { 0., .3, .5, .8 }) {

                model.use(BranchAndBoundOptimizer(
                        DantzigWolfeOptimizer(
                                std_decomposition,
                                GurobiOptimizer::ContinuousRelaxation(),
                                GurobiOptimizer()
                                /*
                                BranchAndBoundOptimizer(
                                        GurobiOptimizer::ContinuousRelaxation(),
                                        MostInfeasible(),
                                        BestBound()
                                )
                                 */
                        ),
                        MostInfeasible(),
                        BestBound()
                ));

                model.set(Param::ColumnGeneration::LogFrequency, 1);
                model.set(Param::ColumnGeneration::BranchingOnMaster, branching_on_master);
                model.set(Param::ColumnGeneration::FarkasPricing, farkas_pricing);
                model.set(Param::ColumnGeneration::SmoothingFactor, smoothing);
                model.set(Param::ColumnGeneration::ArtificialVarCost, 1e5);

                model.optimize();

                std::cout << "RESULT: " << branching_on_master << ", " << farkas_pricing << ", " << smoothing << std::endl;

                std::cout << (SolutionStatus) model.get(Attr::Solution::Status) << std::endl;
                std::cout << (SolutionReason) model.get(Attr::Solution::Reason) << std::endl;
                std::cout << save(model, Attr::Solution::Primal) << std::endl;

                const double obj_val = model.get(Attr::Solution::ObjVal);

                if (obj_val > -39.5) {
                    std::cout << obj_val << std::endl;
                    throw Exception("STOP");
                }

            }

        }

    }

    return 0;
/*
    Logs::set_level<RowGeneration>(Debug);
    Logs::set_color<RowGeneration>(Green);

    Env env;

    BlockModel<Var> model(env, 1);
    auto& subproblem = model.block(0);

    // Master variables
    Var theta(env, 0, Inf, Continuous, "theta");
    Var x(env, 0, Inf, Continuous, "x");

    model.add(theta);
    model.add(x);
    model.master().set(Attr::Obj::Expr, 2 * x + theta);

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

    subproblem.model().set(Attr::Obj::Sense, Maximize);

    model.build_opposite_axis();

    auto& rg = Idol::set_optimizer<RowGeneration>(model);

    rg.set_master_backend<Gurobi>();
    rg.set_subproblem_backend<Gurobi>(0);


    model.optimize();
*/
    return 0;
}
