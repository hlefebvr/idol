#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/callbacks/IntegerMaster.h"
#include "idol/optimizers/wrappers/HiGHS/HiGHS.h"
#include "idol/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/dantzig-wolfe/stabilization/Wentges.h"
#include "idol/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"

using namespace idol;

int main(int t_argc, char** t_argv) {


    const auto instance = Problems::GAP::read_instance("/home/henri/Research/idol/tests/data/generalized-assignment-problem/GAP_instance1.txt");

    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    // Create optimization environment
    Env env;

    Annotation<Ctr> std_decomposition(env, "by_machines", MasterId);

    Annotation<Ctr> nested_decomposition1(env, "nested_decomposition1", MasterId);
    Annotation<Ctr> nested_decomposition2(env, "nested_decomposition2", MasterId);

    Model model(env);

    auto x = Var::make_vector(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add_vector<Var, 2>(x);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {

        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));

        capacity.set(std_decomposition, i);
        capacity.set(nested_decomposition1, i / 2);
        capacity.set(nested_decomposition2, i % 2);

        model.add(capacity);

    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    bool branching_on_sub_problem = true;
    double smoothing_factor = .0;
    bool integer_master_heuristic = true;

    std::unique_ptr<DantzigWolfe::InfeasibilityStrategyFactory> infeasibility_strategy(new DantzigWolfe::ArtificialCosts());

    auto node_solver = std::shared_ptr<OptimizerFactory>(
            DantzigWolfeDecomposition(nested_decomposition1)
                    .with_master_optimizer(HiGHS::ContinuousRelaxation())
                    .with_default_sub_problem_spec(
                            DantzigWolfe::SubProblem()
                                    .add_optimizer(
                                            BranchAndBound<NodeVarInfo>()
                                                    .with_node_optimizer(
                                                            DantzigWolfeDecomposition(nested_decomposition2)
                                                                    .with_master_optimizer(HiGHS::ContinuousRelaxation())
                                                                    .with_default_sub_problem_spec(
                                                                            DantzigWolfe::SubProblem()
                                                                                    .add_optimizer(
                                                                                            BranchAndBound<NodeVarInfo>()
                                                                                                    .with_node_optimizer(
                                                                                                            HiGHS::ContinuousRelaxation())
                                                                                                    .with_branching_rule(MostInfeasible())
                                                                                                    .with_node_selection_rule(BestBound())
                                                                                    )
                                                                    )
                                                                    .with_hard_branching(branching_on_sub_problem)
                                                                    .with_infeasibility_strategy( *infeasibility_strategy)
                                                                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                                                                    .with_log_level(Mute, Cyan)
                                                    )
                                                    .with_branching_rule(MostInfeasible())
                                                    .with_node_selection_rule(BestBound())
                                                    .conditional(integer_master_heuristic, [](auto &x) {
                                                        x.add_callback(
                                                                Heuristics::IntegerMaster()
                                                                        .with_optimizer(HiGHS())
                                                        );
                                                    })
                                                    .with_log_level(Mute, Green)
                                                    .with_log_frequency(1)
                                    )
                    )
                    .with_hard_branching(branching_on_sub_problem)
                    .with_infeasibility_strategy( *infeasibility_strategy)
                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                    .with_log_level(Mute, Yellow)
                    .clone()
    );

    // Set optimizer
    model.use(BranchAndBound()
                      .with_node_optimizer(*node_solver)
                      .with_subtree_depth(0)
                      .with_branching_rule(MostInfeasible())
                      .with_node_selection_rule(BestBound())
                      .add_callback(Heuristics::IntegerMaster().with_optimizer(Gurobi()))
                      .with_log_level(Trace, Blue)
                      .with_log_frequency(1)
    );

    // Solve
    model.optimize();

    std::cout << save_primal(model) << std::endl;

    return 0;
}
