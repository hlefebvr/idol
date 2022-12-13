#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"

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
                idol_Sum(j, Range(n_items), instance.p(i, j) * x[i][j])
            );
    model.set(Attr::Obj::Expr, objective);

    // Knapsack constraints
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        auto ctr = model.add_ctr( idol_Sum(j, Range(n_items), instance.w(i, j) * x[i][j]) <= instance.t(i) );
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

    return 0;
}
