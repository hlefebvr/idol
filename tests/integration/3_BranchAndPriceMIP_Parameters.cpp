//
// Created by henri on 13/02/23.
//

#include "../test_utils.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/column-generation/IntegerMaster.h"
#include "idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"

TEMPLATE_LIST_TEST_CASE("BranchAndPriceMIP: solve Generalized Assignment Problem with different stabilizations and branching schemes",
                        "[integration][backend][solver]",
                        milp_solvers) {

    Env env;

    using namespace Problems::GAP;

    const auto [filename, objective_value] = GENERATE(
            std::make_pair<std::string, double>("GAP_instance0.txt", -233.),
            std::make_pair<std::string, double>("GAP_instance1.txt", -22.),
            std::make_pair<std::string, double>("GAP_instance2.txt", -40.)
    );
    const auto integer_master_heuristic = GENERATE(false, true);
    const auto farkas_pricing = GENERATE(false, true);
    const auto branching_on_master = GENERATE(true, false);
    const double smoothing_factor = GENERATE(0., .3, .5, .8);
    const auto subtree_depth = GENERATE(0, 1);

    const auto instance = read_instance("instances/generalized-assignment-problem/" + filename);
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

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

    std::vector<std::pair<std::string, std::shared_ptr<OptimizerFactory>>> relaxation_solvers = {
            { "CG",
              std::shared_ptr<OptimizerFactory>(
                      DantzigWolfeDecomposition(std_decomposition)
                        .with_master_optimizer(TestType::ContinuousRelaxation())
                        .with_pricing_optimizer(TestType())
                        .with_branching_on_master(branching_on_master)
                        .with_farkas_pricing(farkas_pricing)
                        .with_dual_price_smoothing_stabilization(smoothing_factor)
                        .clone()
            )
            },

            { "CG + B&B",
              std::shared_ptr<OptimizerFactory>(
                      DantzigWolfeDecomposition(std_decomposition)
                              .with_master_optimizer(TestType::ContinuousRelaxation())
                              .with_pricing_optimizer(
                                      BranchAndBound<NodeVarInfo>()
                                              .with_node_optimizer(TestType::ContinuousRelaxation())
                                              .with_branching_rule(MostInfeasible())
                                              .with_node_selection_rule(BestBound())
                              )
                        .with_branching_on_master(branching_on_master)
                        .with_farkas_pricing(farkas_pricing)
                        .with_dual_price_smoothing_stabilization(smoothing_factor)
                        .clone()
              )
            },

            { "CG + nested B&P",
              std::shared_ptr<OptimizerFactory>(
                      DantzigWolfeDecomposition(nested_decomposition1)
                              .with_master_optimizer(TestType::ContinuousRelaxation())
                              .with_pricing_optimizer(
                                      BranchAndBound<NodeVarInfo>()
                                              .with_node_optimizer(
                                                      DantzigWolfeDecomposition(nested_decomposition2)
                                                              .with_master_optimizer(TestType::ContinuousRelaxation())
                                                              .with_pricing_optimizer(
                                                                      BranchAndBound<NodeVarInfo>()
                                                                              .with_node_optimizer(
                                                                                      TestType::ContinuousRelaxation())
                                                                              .with_branching_rule(MostInfeasible())
                                                                              .with_node_selection_rule(BestBound())
                                                              )
                                                              .with_branching_on_master(branching_on_master)
                                                              .with_farkas_pricing(farkas_pricing)
                                                              .with_dual_price_smoothing_stabilization(smoothing_factor)
                                              )
                                              .with_branching_rule(MostInfeasible())
                                              .with_node_selection_rule(BestBound())
                                              .conditional(integer_master_heuristic, [](auto &x) {
                                                  x.with_callback(
                                                          Heuristics::IntegerMaster()
                                                                  .with_optimizer(TestType())
                                                  );
                                              })
                              )
                        .with_branching_on_master(branching_on_master)
                        .with_farkas_pricing(farkas_pricing)
                        .with_dual_price_smoothing_stabilization(smoothing_factor)
                        .clone()
              )
            }
    };

    const auto solver_index = GENERATE(0, 1, 2);

    model.use(
            BranchAndBound<NodeVarInfo>()
                    .with_node_optimizer(*relaxation_solvers[solver_index].second)
                    .with_branching_rule(MostInfeasible())
                    .with_node_selection_rule(BestBound())
                    .with_subtree_depth(subtree_depth)
                    .conditional(integer_master_heuristic, [](auto& x){
                        x.with_callback(
                                Heuristics::IntegerMaster()
                                    .with_optimizer(TestType()
                                )
                            );
                    })
            );

    WHEN("The instance \"" + filename + "\" is solved by branch-and-bound "
            + "with subtree depth of " + std::to_string(subtree_depth)
            + " and" + (integer_master_heuristic ? " with integer master heuristic" : "") + " with relaxation solved by "
            + relaxation_solvers[solver_index].first + " with "
            + (farkas_pricing ? "farkas pricing" : "artificial variables")
            + ", branching applied to " + (branching_on_master ? "master" : "subproblem")
            + " and stabilization factor of " + std::to_string(smoothing_factor)
            ) {

        model.optimize();

        THEN("The solution status is Optimal") {

            CHECK(model.get_status() == Optimal);

        }

        AND_THEN("The objective value is " + std::to_string(objective_value)) {

            CHECK(model.get_best_obj() == Catch::Approx(objective_value));

        }

    }

}