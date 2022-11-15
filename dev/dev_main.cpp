#include <iostream>
#include "modeling.h"
#include "algorithms/logs/Log.h"
#include "modeling/expressions/LinExpr.h"
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
#include "algorithms/branch-and-bound/ActiveNodesManagers_Basic.h"
#include "algorithms/column-generation/ColumnGeneration.h"
#include "algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h"
#include "algorithms/branch-and-bound/NodeUpdators_ByBoundCtr.h"
#include "algorithms.h"
#include "algorithms/solvers/Solvers_GLPK_Simplex.h"
#include "algorithms/callbacks/Callbacks_RowGeneration.h"
#include "algorithms/callbacks/AlgorithmInCallback.h"
#include "problems/KP/KP_Instance.h"
#include "problems/robust/RobustProblem.h"
#include "modeling/expressions/Expr.h"

//// EXAMPLE TAKEN FROM http://metodoscuantitativos.ugr.es/pages/web/vblanco/minlp16/slotv2/!
int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);
    Log::set_color("row-generation", Color::Green);

    Problems::Robust<2> problem;

    Model model;
    auto x = model.add_var(0., 1., Continuous, 0., "x");

    Expr expr(2 * x);
    expr + x;
    expr + 2 * x;
    expr + !x;
    expr + 2 * !x;
    x + expr;
    2 * x + expr;
    !x + expr;
    2 * !x + expr;

    2 * !x + 2 * x;
    !x + 2 * x;
    2 * !x + x;
    !x + x;

    return 0;
}

