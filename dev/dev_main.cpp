#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnMaster.h"

int main(int t_argc, const char** t_argv) {

    using namespace Problems::GAP;

    //auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");
    auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/generated/instance_n3_50__3.txt");

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

    Solvers::Gurobi gurobi(model);
    gurobi.solve();

    std::cout << gurobi.get(Attr::Solution::ObjVal) << std::endl;

    // DW reformulation
    Reformulations::DantzigWolfe result(model, complicating_constraint);

    Logs::set_level<BranchAndBound>(Debug);
    Logs::set_color<BranchAndBound>(Blue);

    Logs::set_level<DantzigWolfe>(Info);
    Logs::set_color<DantzigWolfe>(Yellow);

    BranchAndBound solver;

    //solver.set(Param::BranchAndBound::NodeSelection, NodeSelections::DepthFirst);
    //solver.set(Param::Algorithm::MaxIterations, 10);

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(flatten<Var, 2>(x));
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    auto& dantzig_wolfe = solver.set_solution_strategy<DantzigWolfe>(model, complicating_constraint);

    dantzig_wolfe.set(Param::DantzigWolfe::CleanUpThreshold, 200);
    //dantzig_wolfe.set(Param::DantzigWolfe::FarkasPricing, true);

    auto& master = dantzig_wolfe.set_master_solution_strategy<Solvers::Gurobi>();
    master.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

    for (unsigned int i = 1 ; i <= n_knapsacks ; ++i) {
        dantzig_wolfe.subproblem(i).set_exact_solution_strategy<Solvers::Gurobi>();
        dantzig_wolfe.subproblem(i).set_branching_manager<BranchingManagers::OnMaster>();
    }

    solver.solve();

    std::cout << solver.get(Attr::Solution::ObjVal) << std::endl;
    std::cout << solver.time().count() << std::endl;

    /*

    BranchAndBound solver;

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(branching_candidates);
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    auto& decomposition = solver.set_solution_strategy<Decomposition>();
    decomposition.set_rmp_solution_strategy<Solvers::Gurobi>(result.master_problem());

    auto& column_generation = decomposition.add_generation_strategy<ColGen>();

    for (unsigned int i = 1 ; i <= n_knapsacks ; ++i) {
        auto& sp = column_generation.add_subproblem(result.alpha(i));
        sp.set_exact_solution_strategy<Solvers::Gurobi>(result.subproblem(i));
    }

    solver.solve();

    std::cout << solver.primal_solution().objective_value() << std::endl;
    */
    /*
     OLD STYLE

    // Branch and price
    auto solver = branch_and_price(
            result.master_problem(),
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
