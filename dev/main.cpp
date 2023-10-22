#include "idol/modeling.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/nodes/NodeVarInfo.h"
#include "idol/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/column-generation/IntegerMaster.h"
#include "idol/optimizers/solvers/HiGHS.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"

using namespace idol;

int main(int t_argc, char** t_argv) {

    const auto instance = Problems::GAP::read_instance("/home/henri/Research/idol/unit-tests/data/generalized-assignment-problem/GAP_instance0.txt");
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    const auto integer_master_heuristic = false;
    const auto farkas_pricing = true;
    const auto branching_on_master = true;
    const double smoothing_factor = 0.;
    const auto subtree_depth = 0;

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
        capacity.set(nested_decomposition1, i/2);
        capacity.set(nested_decomposition2, i % 2);

        model.add(capacity);

    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    model.set_obj_expr(idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    model.use(
            BranchAndBound<NodeVarInfo>()
                    .with_node_optimizer(
                            DantzigWolfeDecomposition(nested_decomposition1)
                                    .with_master_optimizer(HiGHS::ContinuousRelaxation())
                                    .with_pricing_optimizer(
                                            BranchAndBound<NodeVarInfo>()
                                                    .with_node_optimizer(
                                                            DantzigWolfeDecomposition(nested_decomposition2)
                                                                    .with_master_optimizer(HiGHS::ContinuousRelaxation())
                                                                    .with_pricing_optimizer(
                                                                            BranchAndBound<NodeVarInfo>()
                                                                                    .with_node_optimizer(
                                                                                            HiGHS::ContinuousRelaxation())
                                                                                    .with_branching_rule(MostInfeasible())
                                                                                    .with_node_selection_rule(BestBound())
                                                                    )
                                                                    .with_branching_on_master(branching_on_master)
                                                                    .with_farkas_pricing(farkas_pricing)
                                                                    .with_dual_price_smoothing_stabilization(smoothing_factor)
                                                                    .with_log_level(Info, Cyan)
                                                                    .with_log_frequency(1)
                                                    )
                                                    .with_branching_rule(MostInfeasible())
                                                    .with_node_selection_rule(BestBound())
                                                    .conditional(integer_master_heuristic, [](auto &x) {
                                                        x.with_callback(
                                                                Heuristics::IntegerMaster()
                                                                        .with_optimizer(HiGHS())
                                                        );
                                                    })
                                                    .with_log_level(Info, Green)
                                                    .with_log_frequency(1)
                                    )
                                    .with_branching_on_master(branching_on_master)
                                    .with_farkas_pricing(farkas_pricing)
                                    .with_dual_price_smoothing_stabilization(smoothing_factor)
                                    .with_log_level(Info, Yellow)
                                    .with_log_frequency(1)
                    )
                    .with_branching_rule(MostInfeasible())
                    .with_node_selection_rule(BestBound())
                    .with_subtree_depth(subtree_depth)
                    .conditional(false, [](auto& x){
                        x.with_callback(
                                Heuristics::IntegerMaster()
                                        .with_optimizer(HiGHS()
                                        )
                        );
                    })
                    .with_log_level(Info, Blue)
                    .with_log_frequency(1)
    );

    model.optimize();

    return 0;
}
