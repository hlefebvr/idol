#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"
#include "algorithms/generation/ColGen.h"

int main(int t_argc, const char** t_argv) {

    using namespace Problems::GAP;

    auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");

    const unsigned int n_knapsacks = instance.n_agents();
    const unsigned int n_items = instance.n_jobs();

    Model model;
    auto complicating_constraint = model.add_user_attr<unsigned int>(0, "complicating_constraint");

    // Variables
    auto x = model.add_vars(Dim<2>(n_knapsacks, n_items), 0., 1., Binary, 0., "x");

    // Objective function
    Expr objective = idol_Sum(
                i, Range(n_knapsacks),
                idol_Sum(j, Range(n_items), instance.cost(i, j) * x[i][j])
            );
    model.set(Attr::Obj::Expr, objective);

    // Knapsack constraints
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        auto ctr = model.add_ctr( idol_Sum(j, Range(n_items), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i) );
        model.set<unsigned int>(complicating_constraint, ctr, i+1);
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_knapsacks), x[i][j]) == 1);
    }

    // DW reformulation
    Reformulations::DantzigWolfe result(model, complicating_constraint);

    // Branching candidates
    std::vector<Var> branching_candidates;
    branching_candidates.reserve(n_entries<Var, 2>(x));

    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        for (unsigned int j = 0 ; j < n_items ; ++j) {
            branching_candidates.emplace_back(model.get<Var>(result.reformulated_variable(), x[i][j]) );
        }
    }

    Log::set_level(Trace);

    BranchAndBound solver;

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(branching_candidates);
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    auto& decomposition = solver.set_solution_strategy<Decomposition>();
    decomposition.set_rmp_solution_strategy<Solvers::Gurobi>(result.restricted_master_problem());

    auto& column_generation = decomposition.add_generation_strategy<ColGen>();

    for (unsigned int i = 1 ; i <= n_knapsacks ; ++i) {
        auto& sp = column_generation.add_subproblem(result.alpha(i));
        sp.set_exact_solution_strategy<Solvers::Gurobi>(result.subproblem(i));
    }

    solver.solve();

    std::cout << solver.primal_solution().objective_value() << std::endl;

    /*
     OLD STYLE

    // Branch and price
    auto solver = branch_and_price(
            result.restricted_master_problem(),
            result.alphas().begin(),
            result.alphas().end(),
            result.subproblems().begin(),
            result.subproblems().end(),
            branching_candidates
            );

    solver.solve();

    std::cout << "Optimum: " << solver.primal_solution().objective_value() << std::endl;

     */

    return 0;
}
