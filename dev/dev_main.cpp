#include "modeling.h"
#include "modeling/models/BlockModel.h"
#include "backends/row-generation/RowGeneration.h"
#include "backends/solvers/Gurobi.h"
#include "backends/parameters/Logs.h"
#include "backends/branch-and-bound-v2/BranchAndBoundV2.h"
#include "backends/solvers/GLPK.h"
#include "backends/branch-and-bound-v2/DefaultOptimizer.h"
#include "backends/branch-and-bound-v2/BranchAndBoundOptimizer.h"
#include "backends/branch-and-bound-v2/ContinuousRelaxation.h"
#include "backends/branch-and-bound-v2/MostInfeasible.h"
#include "backends/branch-and-bound-v2/DepthFirst.h"

class MyNode {
    unsigned int m_id = 0;
};

int main(int t_argc, char** t_argv) {

    Env env;
    Model model(env);

    Var x(env, 0., 1., Continuous, "x");
    model.add(x);
    model.add(Ctr(env, x >= .5));
    model.set(Attr::Obj::Expr, -x);

    model.use(BranchAndBoundOptimizer<MyNode>(
                DefaultOptimizer<GLPK>(),
                ContinuousRelaxation(),
                MostInfeasible(),
                DepthFirst()
            ));

    model.optimize();

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
