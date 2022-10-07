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
#include "algorithms/cut-generation/CutGenerationSP.h"
#include "algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_Dual.h"
#include "algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_IIS.h"
#include "algorithms/branch-and-bound/ActiveNodesManagers_Heap.h"
#include "reformulations/DantzigWolfe.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/column-generation/ColumnGenerators_DantzigWolfeRMP.h"

void solve_with_mip() {
    Model model;
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

    /*
    Model rmp;

    auto x = rmp.add_variable(0., 1., Continuous, 1., "x");
    auto y = rmp.add_variable(0., 1., Continuous, 2., "y");
    auto z = rmp.add_variable(0., 1., Continuous, 3., "z");

    auto c1 = rmp.add_constraint(x + y + z <= 1, "c1");
    auto c2 = rmp.add_constraint(    y + z >= 1, "c2");

    Model subproblem;
    auto illegal_terms = rmp.transform().move(subproblem, { c1, c2 });

    std::cout << rmp << std::endl;

    std::cout << subproblem << std::endl;
     */

    Model sp;
    auto x_0 = sp.add_variable(0., 10., Continuous, 0., "x_0");
    auto x_1 = sp.add_variable(0., 10., Continuous, 0., "x_1");
    auto sp_ctr = sp.add_constraint(-8 * x_0 + 10. * x_1 <= 13.);

    Model rmp;
    auto alpha = rmp.add_variable(0., 1., Continuous, -1 * !x_0 + -1 * !x_1, "alpha");
    auto ctr_rmp = rmp.add_constraint( (-2 * !x_0 + 2 * !x_1) * alpha >= 1., "rmp_ctr");
    auto ctr_con = rmp.add_constraint( alpha == 1 , "rmp_convex");

    BranchAndBound solver;
    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Heap>();
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(std::vector<Var> {x_0, x_1 });

    auto& decomposition = solver.set_solution_strategy<Decomposition>();
    decomposition.set_rmp_solution_strategy<ExternalSolver<Gurobi>>(rmp);
    auto& generation = decomposition.add_generation_strategy<ColumnGeneration>();
    auto& subproblem = generation.add_subproblem(alpha);
    subproblem.set_solution_strategy<ExternalSolver<Gurobi>>(sp);
    auto& generator = subproblem.set_branching_scheme<ColumnGenerationBranchingSchemes::RMP>();

    solver.solve();

    std::cout << solver.primal_solution().status() << std::endl;
    std::cout << solver.primal_solution().objective_value() << std::endl;

    return 0;
}

