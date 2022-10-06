#include <iostream>
#include "modeling.h"
#include "algorithms/logs/Log.h"
#include "modeling/expressions/Expr.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/Nodes_Basic.h"
#include "algorithms/decomposition/Decomposition.h"
#include "solvers/gurobi/Gurobi.h"
#include "algorithms/cut-generation/CutGeneration.h"
#include "algorithms/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBound.h"
#include "algorithms/branch-and-bound/NodeStrategies_Basic.h"
#include "algorithms/cut-generation/subproblems/CutGenerationSubproblem.h"
#include "algorithms/cut-generation/generators/CutGenerator.h"
#include "algorithms/cut-generation/original-space-builder/CutGenerationOriginalSpaceBuilderDual.h"
#include "algorithms/cut-generation/original-space-builder/CutGenerationOriginalSpaceBuilderIIS.h"

void solve_with_mip() {
    Env env;
    Model model(env);
    auto x_1 = model.add_variable(0., Inf, Continuous, 2., "x_1");
    auto x_2 = model.add_variable(0., Inf, Continuous, 3., "x_2");
    auto y = model.add_variable(0., Inf, Integer, 2., "y");
    model.add_constraint(x_1 + 2 * x_2 + y >= 3.);
    model.add_constraint(2 * x_1 + -1 * x_2 + 3 * y >= 4.);

    ExternalSolver<Gurobi> solver(model);
    solver.solve();

    std::cout << "MIP -> " << solver.primal_solution().objective_value() << std::endl;
}

//// EXAMPLE TAKEN FROM http://metodoscuantitativos.ugr.es/pages/web/vblanco/minlp16/slotv2/!
int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);
    Log::set_color("cut-generation", Color::Green);

    Env env;

    Model sp(env);
    auto w_1 = sp.add_variable(0., Inf, Continuous, 0., "w_1");
    auto w_2 = sp.add_variable(0., Inf, Continuous, 0., "w_2");
    sp.add_constraint(     w_1 +  2 * w_2 <= 2.);
    sp.add_constraint( 2 * w_1 + -1 * w_2 <= 3.);

    Model rmp(env);
    auto z = rmp.add_variable(0., Inf, Continuous, 1., "z");
    auto y = rmp.add_variable(0., Inf, Integer, 2., "y");

    BranchAndBound result;

    auto& node_strategy = result.set_node_strategy<NodeStrategy<NodeByBound>>();
    node_strategy.template set_active_node_manager_strategy<ActiveNodeManager_Heap>();
    node_strategy.template set_branching_strategy<MostInfeasible>(std::vector<Var> { y });
    node_strategy.template set_node_updator_strategy<NodeUpdatorByBound>();

    auto& decomposition = result.set_solution_strategy<Decomposition>();
    decomposition.template set_rmp_solution_strategy<ExternalSolver<Gurobi>>(rmp);

    auto& cut_generation = decomposition.template add_generation_strategy<CutGeneration>();
    auto &subproblem = cut_generation.template add_subproblem();
    subproblem.template set_solution_strategy<ExternalSolver<Gurobi>>(sp);
    auto& generator = subproblem.template set_generation_strategy<CutGenerator>(rmp, sp);
    generator.set_original_space_builder<CutGenerationOriginalSpaceBuilderIIS>(rmp);
    generator.set_constant( 3 * w_1 + 4 * w_2 );
    generator.set(z, Expr(), 1.);
    generator.set(y, w_1 + 3 * w_2);

    result.solve();

    std::cout <<"B&B -> " << result.objective_value() << std::endl;

    solve_with_mip();


    return 0;
}

