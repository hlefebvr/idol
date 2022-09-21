#include <iostream>
#include "modeling.h"
#include "solvers/gurobi/Gurobi.h"
#include "solvers/lpsolve/Lpsolve.h"
#include "algorithms/branch-and-bound/BranchAndBound.h"
#include "algorithms/logs/Log.h"
#include "algorithms/branch-and-cut-and-price/DecompositionStrategy.h"
#include "algorithms/branch-and-bound/MostInfeasible.h"
#include "algorithms/branch-and-bound/NodeByBound.h"
#include "algorithms/branch-and-bound/BaseNodeStrategy.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerator.h"
#include "algorithms/branch-and-cut-and-price/ColumnGenerationStrategy.h"
#include "algorithms/branch-and-bound/NodeByBoundStrategy.h"
#include "algorithms/branch-and-cut-and-price/DantzigWolfe_RMP_Strategy.h"

#include "ex2_branch_and_price_gap/Instance.h"

int main() {

    Log::set_level(Trace);
    Log::set_color("branch-and-bound", Color::Blue);
    Log::set_color("column-generation", Color::Yellow);

    return 0;
}

