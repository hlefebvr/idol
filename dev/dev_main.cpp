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
#include "backends/branch-and-bound/Relaxations_Continuous.h"
#include "backends/BranchAndBoundMIP.h"
#include "problems/GAP/GAP_Instance.h"
#include "backends/column-generation/Relaxations_DantzigWolfe.h"
#include "backends/column-generation/ColumnGeneration.h"
#include "backends/BranchAndPriceMIP.h"

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_color<BranchAndBound>(Blue);

    using namespace Problems::GAP;

    const auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Env env;

    auto decomposition = Annotation<Ctr, unsigned int>::make_with_default_value(env, "by_machines", MasterId);

    Model model(env);

    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add_array<Var, 2>(x);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i));
        capacity.set(decomposition, i);
        model.add(capacity);
    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1);
        model.add(assignment);
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    Idol::using_backend<BranchAndPriceMIP<Gurobi>>(model, decomposition);

    Relaxations::DantzigWolfe dw(model, decomposition);
    dw.build();

    std::cout << dw.model() << std::endl;

    //model.optimize();

    return 0;

}
