#include <iostream>
#include "modeling.h"
#include "problems/FLP/FLP_Instance.h"
#include "backends/solvers/Gurobi.h"
#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/branch-and-bound/NodeStrategies_Basic.h"
#include "backends/branch-and-bound/Nodes_Basic.h"
#include "backends/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "backends/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "backends/branch-and-bound/NodeUpdators_ByBoundVar.h"

namespace Relaxations {
    class None;
}

struct Relaxations::None {
    static void apply(Model& t_model) {}
};

namespace Relaxations {
    class Continuous;
}

struct Relaxations::Continuous {

    static std::vector<Var> apply(Model& t_model) {

        std::vector<Var> result;

        for (const auto& var : t_model.vars()) {

            if (const int type = t_model.get(Attr::Var::Type, var) ; type == Integer || type == Binary) {

                result.emplace_back(var);
                t_model.set(Attr::Var::Type, var, ::Continuous);

            }

        }

        return result;
    }
};

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_color<BranchAndBound>(Blue);

    auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/CLionProjects/idol_robust_binary/DisruptionFLP/data/instance_F15_C70__1.txt");
    //auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/CLionProjects/optimize/dev/flp_instance.txt");

    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    Env env;

    auto x = Var::array(env, Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = Var::array(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, "y");

    Model model(env);

    model.add<Var, 1>(x);
    model.add<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers), instance.per_unit_transportation_cost(i, j) * instance.demand(j) * y[i][j])));

    model.set_backend<Gurobi>();

    model.optimize();

    std::cout << "Gurobi: " << model.get(Attr::Solution::ObjVal) << std::endl;

    model.reset_backend();

    auto& branch_and_bound = model.set_backend<BranchAndBound>();
    auto branching_candidates = branch_and_bound.relax<Relaxations::Continuous>();
    branch_and_bound.set_node_backend<Gurobi>();
    auto& nodes_manager = branch_and_bound.set_nodes_manager<NodeStrategies::Basic<Nodes::Basic>>();
    nodes_manager.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    nodes_manager.set_branching_strategy<BranchingStrategies::MostInfeasible>(branching_candidates);
    nodes_manager.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    model.optimize();

    std::cout << "B&B: " << model.get(Attr::Solution::BestObj) << std::endl;

    /*
    std::cout << (SolutionStatus) model.get(Attr::Solution::Status) << std::endl;
    std::cout << (Reason) model.get(Attr::Solution::Reason) << std::endl;
    std::cout << model.get(Attr::Solution::ObjVal) << std::endl;
    std::cout << model.get(Attr::Var::Value, x[0]) << std::endl;

    std::cout << save(model, Attr::Var::Value) << std::endl;

     */

    return 0;

    /*
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
     */
}
