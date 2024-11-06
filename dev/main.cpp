#include <iostream>
#include "idol/general/utils/SparseVector.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/mixed-integer/modeling/objects/Env.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h"
#include "idol/mixed-integer/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h"
#include <gurobi_c++.h>

using namespace idol;

#define OPTIMIZER GLPK

int main(int t_argc, const char** t_argv) {

    using namespace Problems::GAP;

    Env env;

    const auto [filename, objective_value] = std::make_pair<std::string, double>("GAP_instance2.txt", -40.);
    const auto integer_master_heuristic = false;
    const auto branching_on_sub_problem = true;
    const double smoothing_factor = .5;
    const auto subtree_depth = 0;
    const auto solver_index = 2;

    const auto instance = read_instance("../data/generalized-assignment-problem/" + filename);
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Annotation<Ctr> std_decomposition(env, "by_machines", MasterId);

    Annotation<Ctr> nested_decomposition1(env, "nested_decomposition1", MasterId);
    Annotation<Ctr> nested_decomposition2(env, "nested_decomposition2", MasterId);

    const auto farkas_pricing = false;

    std::unique_ptr<DantzigWolfe::InfeasibilityStrategyFactory> infeasibility_strategy(farkas_pricing ?
        (DantzigWolfe::InfeasibilityStrategyFactory*) new DantzigWolfe::FarkasPricing() :
        (DantzigWolfe::InfeasibilityStrategyFactory*) new DantzigWolfe::ArtificialCosts()
    );

    Model model(env);

    auto x = Var::make_vector(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, 0., "x");
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

    std::vector<std::pair<std::string, std::shared_ptr<OptimizerFactory>>> relaxation_solvers = {
            { "CG",
                    std::shared_ptr<OptimizerFactory>(
                            DantzigWolfeDecomposition(std_decomposition)
                                    .with_master_optimizer(OPTIMIZER::ContinuousRelaxation())
                                    .with_default_sub_problem_spec(
                                            DantzigWolfe::SubProblem()
                                                    .add_optimizer(OPTIMIZER())
                                    )
                                    .with_hard_branching(branching_on_sub_problem)
                                    .with_infeasibility_strategy( *infeasibility_strategy)
                                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                                    .clone()
                    )
            },

            { "CG + B&B",
                    std::shared_ptr<OptimizerFactory>(
                            DantzigWolfeDecomposition(std_decomposition)
                                    .with_master_optimizer(OPTIMIZER::ContinuousRelaxation())
                                    .with_default_sub_problem_spec(
                                            DantzigWolfe::SubProblem()
                                                    .add_optimizer(
                                                            BranchAndBound<DefaultNodeInfo>()
                                                                    .with_node_optimizer(OPTIMIZER::ContinuousRelaxation())
                                                                    .with_branching_rule(MostInfeasible())
                                                                    .with_node_selection_rule(BestBound())
                                                    )
                                    )
                                    .with_hard_branching(branching_on_sub_problem)
                                    .with_infeasibility_strategy( *infeasibility_strategy)
                                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                                    .clone()
                    )
            },

            { "CG + nested B&P",
                    std::shared_ptr<OptimizerFactory>(
                            DantzigWolfeDecomposition(nested_decomposition1)
                                    .with_master_optimizer(OPTIMIZER::ContinuousRelaxation())
                                    .with_default_sub_problem_spec(
                                            DantzigWolfe::SubProblem()
                                                    .add_optimizer(
                                                            BranchAndBound<DefaultNodeInfo>()
                                                                    .with_node_optimizer(
                                                                            DantzigWolfeDecomposition(nested_decomposition2)
                                                                                    .with_master_optimizer(OPTIMIZER::ContinuousRelaxation())
                                                                                    .with_default_sub_problem_spec(
                                                                                            DantzigWolfe::SubProblem()
                                                                                                    .add_optimizer(
                                                                                                            BranchAndBound<DefaultNodeInfo>()
                                                                                                                    .with_node_optimizer(
                                                                                                                            OPTIMIZER::ContinuousRelaxation())
                                                                                                                    .with_branching_rule(MostInfeasible())
                                                                                                                    .with_node_selection_rule(BestBound())
                                                                                                    )
                                                                                    )
                                                                                    .with_hard_branching(branching_on_sub_problem)
                                                                                    .with_infeasibility_strategy( *infeasibility_strategy)
                                                                                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                                                                                    .with_logs(false)
                                                                    )
                                                                    .with_branching_rule(MostInfeasible())
                                                                    .with_node_selection_rule(BestBound())
                                                                    .conditional(integer_master_heuristic, [](auto &x) {
                                                                        x.add_callback(
                                                                                Heuristics::IntegerMaster()
                                                                                        .with_optimizer(OPTIMIZER().with_presolve(false))
                                                                        );
                                                                    })
                                                                    .with_logs(false)
                                                    )
                                    )
                                    .with_hard_branching(branching_on_sub_problem)
                                    .with_infeasibility_strategy( *infeasibility_strategy)
                                    .with_dual_price_smoothing_stabilization(DantzigWolfe::Neame(smoothing_factor))
                                    .with_logs(false)
                                    .clone()
                    )
            }
    };

    model.use(
            BranchAndBound<DefaultNodeInfo>()
                    .with_node_optimizer(*relaxation_solvers[solver_index].second)
                    .with_branching_rule(MostInfeasible())
                    .with_node_selection_rule(BestBound())
                    .with_subtree_depth(subtree_depth)
                    .conditional(integer_master_heuristic, [](auto& x){
                        x.add_callback(
                                Heuristics::IntegerMaster()
                                        .with_optimizer(OPTIMIZER().with_presolve(false))
                        );
                    })
                    .with_logs(true)
    );


    model.optimize();

    /*

    Env env;
    Model model(env, Model::Storage::RowOriented);
    const auto x = model.add_vars(Dim<1>(10), 0., 1., Continuous, 1, "x");

    LinExpr<Ctr> col;
    model.add_var(0, 10, Continuous, 1, col);

    for (unsigned int i = 0; i < 9; ++i) {
        model.add_ctr(x[i] + x[i+1] >= 0.5);
    }

    //std::cout << model << std::endl;

    const auto column = model.get_var_column(x[3]);

    model.dump();

    model.set_storage(Model::Storage::ColumnOriented, true);

    const auto& row = model.get_ctr_row(model.get_ctr_by_index(2));

    model.dump();

    model.set_storage(Model::Storage::Both);

    model.dump();
     */

    return 0;
}
