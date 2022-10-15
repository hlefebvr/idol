#include <iostream>
#include "modeling.h"
#include "algorithms/logs/Log.h"
#include "modeling/expressions/Expr.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/branch-and-bound/Nodes_Basic.h"
#include "algorithms/decomposition/Decomposition.h"
#include "algorithms/row-generation/RowGeneration.h"
#include "algorithms/branch-and-bound/BranchingStrategies_MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundVar.h"
#include "algorithms/branch-and-bound/NodeStrategies_Basic.h"
#include "algorithms/row-generation/RowGenerationSP.h"
#include "algorithms/row-generation/RowGenerationOriginalSpaceBuilders_Dual.h"
#include "algorithms/row-generation/RowGenerationOriginalSpaceBuilders_IIS.h"
#include "algorithms/branch-and-bound/ActiveNodesManagers_Heap.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundCtr.h"
#include "algorithms.h"
#include "algorithms/solvers/Solvers_GLPK_Simplex.h"
#include "algorithms/callbacks/Callbacks_RowGeneration.h"
#include "algorithms/callbacks/AlgorithmInCallback.h"
#include "problems/kp/KP_Instance.h"

//// EXAMPLE TAKEN FROM http://metodoscuantitativos.ugr.es/pages/web/vblanco/minlp16/slotv2/!
int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);
    Log::set_color("row-generation", Color::Green);

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

    auto x = model.add_variable(0., 1., Binary, 0.);

    std::cout << Env::get().size() << std::endl;

    model.remove(x);

    std::cout << Env::get().size() << std::endl;

    std::cout << x.status() << std::endl;


    return 0;
}

