#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "reformulations/Reformulations_Benders.h"
#include "algorithms/benders/Benders.h"
#include "problems/FLP/FLP_Instance.h"
#include "modeling/models/Env.h"
#include "algorithms/benders/Nodes_BendersIIS.h"

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound>(Trace);
    Logs::set_color<BranchAndBound>(Blue);
    Logs::set_level<Benders>(Trace);
    Logs::set_color<Benders>(Yellow);

    auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/CLionProjects/optimize/dev/flp_instance.txt");

    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    {
        Model direct;
        auto y = direct.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "y");
        auto x = direct.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "x");

        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            direct.add_ctr(idol_Sum(i, Range(n_facilities), x[i][j]) == 1);
        }

        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            for (unsigned int i = 0 ; i < n_facilities ; ++i) {
                direct.add_ctr(x[i][j] <= y[i]);
            }
        }

        for (unsigned int i = 0 ; i < n_facilities ; ++i) {
            direct.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * x[i][j]) <= instance.capacity(i) * y[i]);
        }

        direct.set(Attr::Obj::Expr, idol_Sum(
                    i, Range(n_facilities),
                    instance.fixed_cost(i) * y[i]
                    + idol_Sum(j, Range(n_customers),
                      instance.demand(j) * instance.per_unit_transportation_cost(i, j) * x[i][j]
                     )
                ));

        Solvers::Gurobi gurobi(direct);
        gurobi.solve();

        std::cout << gurobi.primal_solution() << std::endl;
    }

    Model subproblem(Env::global(), Maximize);
    auto u = subproblem.add_vars(Dim<1>(n_customers), 0., Inf, Continuous, 0., "u");
    auto v = subproblem.add_vars(Dim<2>(n_facilities, n_customers), 0., Inf, Continuous, 0., "v");
    auto w = subproblem.add_vars(Dim<1>(n_facilities), 0., Inf, Continuous, 0., "w");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            subproblem.add_ctr(u[j] - v[i][j] - instance.demand(j) * w[i] <= instance.demand(j) * instance.per_unit_transportation_cost(i, j));
        }
    }

    Model rmp;
    auto z = rmp.add_var(0., Inf, Continuous, 1, "z");
    auto y = rmp.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "y");
    Expr expr;
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        expr -= instance.capacity(i) * !w[i] * y[i];
        for (unsigned int j = 0; j < n_customers; ++j) {
            expr -= !v[i][j] * y[i];
        }
    }
    for (unsigned int j = 0; j < n_customers; ++j) {
        expr += !u[j];
    }
    auto cut = rmp.add_ctr(z >= expr);
    rmp.set(Attr::Obj::Expr, idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * y[i]) + z);

    Benders solver(rmp, true);

    //solver.set(Param::Algorithm::MaxIterations, 0);

    auto& master_solver = solver.set_master_solution_strategy<Solvers::Gurobi>();
    master_solver.set(Param::Algorithm::ResetBeforeSolving, true);

    auto& benders_subproblem = solver.add_subproblem(subproblem, cut, z);
    benders_subproblem.set_exact_solution_strategy<Solvers::Gurobi>();
    benders_subproblem.exact_solution_strategy().set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

    solver.solve();

    std::cout << "--------------" << std::endl;
    std::cout << solver.primal_solution() << std::endl;

    return 0;
}
