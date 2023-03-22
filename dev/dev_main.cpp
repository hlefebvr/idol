#include "modeling.h"
#include "modeling/models/BlockModel.h"
#include "backends/row-generation/RowGeneration.h"
#include "backends/solvers/Gurobi.h"
#include "backends/parameters/Logs.h"
#include "backends/branch-and-bound-v2/BranchAndBoundV2.h"
#include "backends/solvers/GLPK.h"
#include "backends/solvers/DefaultOptimizer.h"
#include "backends/branch-and-bound-v2/BranchAndBoundOptimizer.h"
#include "backends/branch-and-bound-v2/relaxations/impls/ContinuousRelaxation.h"
#include "backends/branch-and-bound-v2/branching-rules/factories/MostInfeasible.h"
#include "backends/branch-and-bound-v2/node-selection-rules/factories/DepthFirst.h"
#include "problems/generalized-assignment-problem/GAP_Instance.h"
#include "problems/facility-location-problem/FLP_Instance.h"

class MyNode {
    unsigned int m_id = 0;
};

int main(int t_argc, char** t_argv) {

    Logs::set_level<BranchAndBoundV2<NodeInfo>>(Trace);
    Logs::set_color<BranchAndBoundV2<NodeInfo>>(Blue);

    // Read instance
    const auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/CLionProjects/optimize/tests/instances/facility-location-problem/instance_F10_C20__4.txt");
    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    Env env;

    // Make model
    auto x = Var::array(env, Dim<1>(n_facilities), 0., 1., Binary, "x");
    auto y = Var::array(env, Dim<2>(n_facilities, n_customers), 0., 1., Continuous, "y");

    Model model(env);

    model.add_array<Var, 1>(x);
    model.add_array<Var, 2>(y);

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        model.add(Ctr(env, idol_Sum(j, Range(n_customers), instance.demand(j) * y[i][j]) <= instance.capacity(i) * x[i]));
    }

    for (unsigned int j = 0 ; j < n_customers ; ++j) {
        model.add(Ctr(env, idol_Sum(i, Range(n_facilities), y[i][j]) == 1));
    }

    model.set(Attr::Obj::Expr, idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * x[i] + idol_Sum(j, Range(n_customers), instance.per_unit_transportation_cost(i, j) * instance.demand(j) * y[i][j])));


    model.use(BranchAndBoundOptimizer<NodeInfo>(
                DefaultOptimizer<Gurobi>(),
                ContinuousRelaxation(),
                MostInfeasible(),
                DepthFirst()
            ));

    model.optimize();

    std::cout << (SolutionStatus) model.get(Attr::Solution::Status) << std::endl;
    std::cout << (SolutionReason) model.get(Attr::Solution::Reason) << std::endl;
    std::cout << save(model, Attr::Solution::Primal) << std::endl;

    return 0;
/*
    Logs::set_level<RowGeneration>(Debug);
    Logs::set_color<RowGeneration>(Green);

    Env env;

    BlockModel<Var> model(env, 1);
    auto& subproblem = model.block(0);

    // Master variables
    Var theta(env, 0, Inf, Continuous, "theta");
    Var x(env, 0, Inf, Continuous, "x");

    model.add(theta);
    model.add(x);
    model.master().set(Attr::Obj::Expr, 2 * x + theta);

    // Subproblem variables
    auto lambda = Var::array(env, Dim<1>(2), 0, Inf, Continuous, "lambda");
    subproblem.model().add_array<Var, 1>(lambda);

    // Subproblem constraints
    Ctr c1(env, lambda[0] + 2 * lambda[1] <= 2.);
    subproblem.model().add(c1);

    Ctr c2(env, 2 * lambda[0] - lambda[1] <= 3.);
    subproblem.model().add(c2);

    // Generation pattern
    subproblem.generation_pattern() = Row(theta + (!lambda[0] + 3. * !lambda[1]) * x,  3. * !lambda[0] + 4. * !lambda[1]);

    // Set epigraph
    subproblem.set_aggregator(theta);

    subproblem.model().set(Attr::Obj::Sense, Maximize);

    model.build_opposite_axis();

    auto& rg = Idol::set_optimizer<RowGeneration>(model);

    rg.set_master_backend<Gurobi>();
    rg.set_subproblem_backend<Gurobi>(0);


    model.optimize();
*/
    return 0;
}
