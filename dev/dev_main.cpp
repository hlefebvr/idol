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
#include "modeling/models/Decomposition.h"
#include "backends/BranchAndBoundMIP.h"
#include "problems/GAP/GAP_Instance.h"
#include "backends/column-generation/Relaxations_DantzigWolfe.h"

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBound>(Info);
    Logs::set_color<BranchAndBound>(Blue);

    using namespace Problems::GAP;

    const auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");
    const unsigned int n_agents = instance.n_agents();
    const unsigned int n_jobs = instance.n_jobs();

    Env env;

    Annotation<Ctr, unsigned int> subproblem(env, "subproblem");

    Model model(env);

    auto x = Var::array(env, Dim<2>(n_agents, n_jobs), 0., 1., Binary, "x");
    model.add<Var, 2>(x);

    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        Ctr capacity(env, idol_Sum(j, Range(n_jobs), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i));
        capacity.set(subproblem, i);
        model.add(capacity);
    }

    for (unsigned int j = 0 ; j < n_jobs ; ++j) {
        Ctr assignment(env, idol_Sum(i, Range(n_agents), x[i][j]) == 1);
        assignment.set(subproblem, MasterId);
        model.add(assignment);
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_agents), idol_Sum(j, Range(n_jobs), instance.cost(i, j) * x[i][j])));

    Relaxations::DantzigWolfe dantzig_wolfe(model, subproblem);
    dantzig_wolfe.build();

    std::cout << dantzig_wolfe.decomposition().master_problem() << std::endl;
    for (unsigned int i = 0 ; i < n_agents ; ++i) {
        std::cout << "Pricing " << i << std::endl;
        std::cout << dantzig_wolfe.decomposition().subproblem(i) << std::endl;
        std::cout << "Column " << i << std::endl;
        std::cout << dantzig_wolfe.decomposition().generation_pattern(i) << std::endl;
    }

    return 0;

}
