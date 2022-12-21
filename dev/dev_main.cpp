#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnMaster.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnPricing.h"
#include "algorithms/dantzig-wolfe/Attributes_DantzigWolfe.h"
#include "algorithms/callbacks/Callbacks_PlotOptimalityGap.h"

#include "algorithms/callbacks/Algorithm_Events.h"
#include "algorithms/callbacks/Callbacks_RoundingHeuristic.h"
#include "algorithms/callbacks/Callbacks_FeasibilityPump.h"

#include <TApplication.h>

int main(int t_argc, char** t_argv) {

    TApplication app("app", &t_argc, t_argv);

    using namespace Problems::GAP;

    //auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");
    //auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/tiny/instance_n2_30__2.txt");
    auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/small/instance_n3_40__0.txt");

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

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_color<BranchAndBound>(Blue);

    Logs::set_level<DantzigWolfe>(Debug);
    Logs::set_color<DantzigWolfe>(Yellow);

    //Logs::set_level<Callbacks::FeasiblityPump>(Trace);;

    BranchAndBound solver;

    //solver.set_user_callback<Callbacks::PlotOptimalityGap>();
    solver.set_user_callback<Callbacks::FeasibilityPump>(model);

    for (const auto& x_ij : flatten<Var, 2>(x)) {
        model.set(Attr::Var::Type, x_ij, Continuous);
    }

    solver.set(Param::BranchAndBound::NodeSelection, NodeSelections::DepthFirst);
    //solver.set(Param::Algorithm::MaxIterations, 10);

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(flatten<Var, 2>(x));
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    solver.set_solution_strategy<Solvers::Gurobi>(model);

    solver.set(Param::Algorithm::TimeLimit, 600);
    solver.set(Param::Algorithm::MaxIterations, 100000);

    solver.solve();

    std::cout << solver.get(Attr::Solution::ObjVal) << std::endl;
    std::cout << solver.time().count() << std::endl;

    //app.Run();

    return 0;
}
