//
// Created by henri on 21/12/22.
//
#include <iostream>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnMaster.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnPricing.h"
#include "algorithms/callbacks/Callbacks_PlotOptimalityGap.h"

#include "algorithms/callbacks/Callbacks_FeasibilityPump.h"
#include "algorithms/callbacks/Callbacks_IntegerMasterProblem.h"

//#include <TApplication.h>

int main(int t_argc, char** t_argv) {

    //TApplication app("app", &t_argc, t_argv);

    using namespace Problems::GAP;

    //auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");
    //auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/tiny/instance_n2_30__2.txt");
    auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/n3/instance_n3_40__2.txt");

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

    Solvers::GLPK gurobi(model);
    gurobi.solve();

    std::cout << gurobi.get(Attr::Solution::ObjVal) << std::endl;

    // DW reformulation
    Reformulations::DantzigWolfe result(model, complicating_constraint);

    Logs::set_level<BranchAndBound>(Trace);
    Logs::set_color<BranchAndBound>(Blue);

    Logs::set_level<DantzigWolfe>(Debug);
    Logs::set_color<DantzigWolfe>(Yellow);

    Logs::set_level<Callbacks::FeasibilityPump>(Trace);;

    BranchAndBound solver;

    //solver.set_user_callback<Callbacks::PlotOptimalityGap>();

    //solver.set(Param::BranchAndBound::NodeSelection, NodeSelections::DepthFirst);
    //solver.set(Param::Algorithm::MaxIterations, 10);

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(flatten<Var, 2>(x));
    node_strategy.set_node_updator<NodeUpdators::ByBoundVar>();

    auto& dantzig_wolfe = solver.set_solution_strategy<DantzigWolfe>(model, complicating_constraint);

    //dantzig_wolfe.set_user_callback<Callbacks::PlotOptimalityGap>();

    dantzig_wolfe.set(Param::DantzigWolfe::CleanUpThreshold, 1500);
    dantzig_wolfe.set(Param::DantzigWolfe::SmoothingFactor, 0);
    dantzig_wolfe.set(Param::DantzigWolfe::FarkasPricing, true);
    dantzig_wolfe.set(Param::DantzigWolfe::LogFrequency, 1);

    auto& master = dantzig_wolfe.set_master_solution_strategy<Solvers::GLPK>();
    master.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

    for (unsigned int i = 1 ; i <= n_knapsacks ; ++i) {
        dantzig_wolfe.subproblem(i).set_exact_solution_strategy<Solvers::GLPK>();
        dantzig_wolfe.subproblem(i).set_branching_manager<BranchingManagers::OnPricing>();
    }

    //solver.add_callback<Callbacks::RoundingHeuristic>(flatten<Var, 2>(x));
    solver.set_user_callback<Callbacks::IntegerMasterProblem>();

    solver.set(Param::Algorithm::TimeLimit, 600);
    solver.set(Param::Algorithm::MaxIterations, 100000);

    solver.solve();

    std::cout << solver.get(Attr::Solution::ObjVal) << std::endl;
    std::cout << solver.time().count() << std::endl;
    std::cout << solver.primal_solution() << std::endl;

    //app.Run();

    return 0;
}
