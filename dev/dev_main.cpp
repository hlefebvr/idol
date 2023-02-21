#include <iostream>
#include <Eigen/Eigen>
#include "modeling.h"
#include "problems/facility-location-problem/FLP_Instance.h"
#include "backends/solvers/Gurobi.h"
#include "backends/branch-and-bound/BranchAndBound.h"
#include "backends/branch-and-bound/NodeStrategies_Basic.h"
#include "backends/branch-and-bound/Nodes_Basic.h"
#include "backends/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "backends/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "backends/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "backends/branch-and-bound/Relaxations_Continuous.h"
#include "backends/BranchAndBoundMIP.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "backends/column-generation/Relaxations_DantzigWolfe.h"
#include "backends/column-generation/ColumnGeneration.h"
#include "backends/BranchAndPriceMIP.h"
#include "backends/solvers/GLPK.h"
#include "backends/solvers/Mosek.h"

int main(int t_argc, char** t_argv) {

    Env env;
    Model model(env);

    auto x = Var::array<1>(env, Dim<1>(3), 0., Inf, Continuous, "x");
    model.add_array<Var, 1>(x);

    Ctr quadratic(env, x[0] * x[0] + x[1] * x[1] <= x[2] * x[2]);
    model.add(quadratic);

    model.set(Attr::Obj::Expr, -x[0] - x[1]);

    Idol::set_optimizer<Mosek>(model);

    model.update();

    return 0;
/*
    Logs::set_level<BranchAndBound>(Debug);
    Logs::set_color<BranchAndBound>(Blue);

    Logs::set_level<ColumnGeneration>(Debug);
    Logs::set_color<ColumnGeneration>(Yellow);

    using namespace Problems::GAP;

    const auto instance = read_instance("/home/henri/CLionProjects/optimize/tests/instances/generalized-assignment-problem/GAP_instance0.txt");
    //const auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/n3/instance_n3_30__3.txt");
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Env env;

    Annotation<Ctr> decomposition(env, "by_machines", MasterId);
    Annotation<Ctr> decomposition2(env, "by_machines", MasterId);

    Model model(env);

    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add_array<Var, 2>(x);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i), "capacity_" + std::to_string(i));
        capacity.set(decomposition, i);
        capacity.set(decomposition2, 0);
        model.add(capacity);
    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1, "assignment_" + std::to_string(j));
        model.add(assignment);
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    Idol::set_optimizer<BranchAndPriceMIP<Mosek>>(model, decomposition);

    model.set(Param::Algorithm::MaxThreads, 1);
    model.set(Param::Algorithm::TimeLimit, 600);
    model.set(Param::BranchAndBound::LogFrequency, 1);
    model.set(Param::ColumnGeneration::LogFrequency, 1);
    model.set(Param::ColumnGeneration::FarkasPricing, false);
    model.set(Param::ColumnGeneration::BranchingOnMaster, true);
    model.set(Param::ColumnGeneration::SmoothingFactor, .3);

    model.optimize();

    std::cout << save(model, Attr::Solution::Primal) << std::endl;
*/
    return 0;
}
