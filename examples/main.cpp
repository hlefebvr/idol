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
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "algorithms/branch-and-bound/NodeStrategies_Basic.h"
#include "algorithms/cut-generation/CutGenerationSP.h"
#include "algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_Dual.h"
#include "algorithms/cut-generation/CutGenerationOriginalSpaceBuilders_IIS.h"
#include "algorithms/branch-and-bound/ActiveNodesManagers_Heap.h"
#include "reformulations/DantzigWolfe.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundCtr.h"
#include "solvers/glpk/GLPK.h"
#include "algorithms.h"

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

    Model model;

    auto x = model.add_variable(0., Inf, Continuous, -3, "x");
    auto y = model.add_variable(0., Inf, Continuous, -2, "y");
    auto c1 = model.add_constraint(x + -2 * y <= 1);
    auto c2 = model.add_constraint(-2 * x + y <= 1);
    auto c3 = model.add_constraint(x + y >= 2);

    GLPK solver(model);
    solver.set_infeasible_or_unbounded_info(true);
    solver.solve();

    auto ray = solver.unbounded_ray();

    std::cout << -3 * ray.get(x) -2 * ray.get(y) << std::endl;
    std::cout << ray.get(x) - 2 * ray.get(y) << std::endl;
    std::cout << -2 * ray.get(x) + ray.get(y) << std::endl;
    std::cout << ray.get(x) + ray.get(y) << std::endl;

    std::cout << ray << std::endl;

    return 0;
}

