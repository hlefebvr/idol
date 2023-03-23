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

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound<NodeInfo>>(Trace);
    Logs::set_color<BranchAndBound<NodeInfo>>(Blue);

    Logs::set_level<ColumnGeneration>(Info);
    Logs::set_color<ColumnGeneration>(Yellow);

    // Read instance
    const auto instance = Problems::GAP::read_instance("/home/henri/CLionProjects/optimize/tests/instances/generalized-assignment-problem/GAP_instance0.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();


    Env env;

    // Make model
    Model model(env);

    // Create decomposition annotation
    Annotation<Ctr> decomposition(env, "decomposition", MasterId);
    Annotation<Ctr> decomposition2(env, "decomposition2", MasterId);

    // Create assignment variables (x_ij binaries)
    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");

    // Add variables to the model
    model.add_array<Var, 2>(x);

    // Create knapsack constraints (i.e., capacity constraints)
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        model.add(capacity);

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

    model.use(BranchAndBoundOptimizer<NodeInfo>(
                ColumnGenerationOptimizer(
                    DefaultOptimizer<Gurobi>(),
                    BranchAndBoundOptimizer<NodeInfo>(
                        ColumnGenerationOptimizer(
                                DefaultOptimizer<Gurobi>(),
                                BranchAndBoundOptimizer<NodeInfo>(
                                    DefaultOptimizer<GLPK>(),
                                    ContinuousRelaxation(),
                                    MostInfeasible(),
                                    WorstBound()
                                )
                        ),
                        DantzigWolfeRelaxation(decomposition2),
                        MostInfeasible(),
                        DepthFirst()
                    )
                ),
                DantzigWolfeRelaxation(decomposition),
                MostInfeasible(),
                BestBound()
            ));

    using SolverT = Gurobi;

    std::unique_ptr<OptimizerFactory> subproblem_optimizer(new BranchAndBoundOptimizer<NodeInfo>(
                DefaultOptimizer<SolverT>(),
                ContinuousRelaxation(),
                MostInfeasible(),
                BestBound()
            ));

    model.use(BranchAndBoundOptimizer(
                    ColumnGenerationOptimizer(
                            DefaultOptimizer<SolverT>(),
                            DefaultOptimizer<SolverT>()
                    ),
                    DantzigWolfeRelaxation(decomposition),
                    MostInfeasible(),
                    BestBound()
            ));

    for (bool branching_on_master : { true, false }) {

        for (bool farkas_pricing : { true, false }) {

            for (double smoothing : { 0., .3, .8 }) {


                model.set(Param::ColumnGeneration::LogFrequency, 1);
                model.set(Param::ColumnGeneration::BranchingOnMaster, branching_on_master);
                model.set(Param::ColumnGeneration::FarkasPricing, farkas_pricing);
                model.set(Param::ColumnGeneration::SmoothingFactor, smoothing);

                model.optimize();

                std::cout << "RESULT: " << branching_on_master << ", " << farkas_pricing << ", " << smoothing << std::endl;

                std::cout << (SolutionStatus) model.get(Attr::Solution::Status) << std::endl;
                std::cout << (SolutionReason) model.get(Attr::Solution::Reason) << std::endl;
                std::cout << save(model, Attr::Solution::Primal) << std::endl;

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
