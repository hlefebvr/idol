#include "OsiSolverInterface.hpp"
#include "OsiClpSolverInterface.hpp"
#include "OsiSymSolverInterface.hpp"

#include "MibSModel.hpp"

#include "AlpsKnowledgeBrokerSerial.h"
#include "idol/modeling.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Osi/OsiIdolSolverInterface.h"
#include "idol/problems/knapsack-problem/KP_Instance.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Osi/Osi.h"
#include "idol/problems/facility-location-problem/FLP_Instance.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/BranchAndBound.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/PseudoCost.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/cutting-planes/KnapsackCover.h"
#include "idol/problems/generalized-assignment-problem/GAP_Instance.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/logs/Info.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/stabilization/Neame.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/optimizers/mixed-integer-optimization/branch-and-bound/node-selection-rules/factories/WorstBound.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/heuristics/IntegerMaster.h"
#include "idol/optimizers/mixed-integer-optimization/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Bilevel.h"
#include "idol/optimizers/bilevel-optimization/wrappers/MibS/MibS.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/separators/Dualize.h"
#include "idol/modeling/bilevel-optimization/read_from_file.h"
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/ColumnAndConstraintGeneration.h"
#include "idol/optimizers/bilevel-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h"
#include "idol/optimizers/robust-optimization/column-and-constraint-generation/stabilizers/NoStabilization.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Mosek/Mosek.h"
#include "idol/modeling/new-matrix/SparseMatrix.h"

#include <iostream>
#include <OsiCpxSolverInterface.hpp>
#include <OsiCbcSolverInterface.hpp>

void hello_world_osi_idol() {

    using namespace idol;

    Env env;
    Model model(env);
    model.use(Gurobi().with_logs(true));

    OsiIdolSolverInterface interface(model);
    std::unique_ptr<OsiSolverInterface> solver(interface.clone());
    // solver->messageHandler()->setLogLevel(0);

    /*
    *  optimal solution: x* = (1,1)
    *
    *  minimize -1 x0 - 1 x1
    *  s.t       1 x0 + 2 x1 <= 3
    *            2 x0 + 1 x1 <= 3
    *              x0        >= 0
    *              x1        >= 0
    */

    solver->addCol(CoinPackedVector(), 0., solver->getInfinity(), -1.);
    solver->addCol(CoinPackedVector(), 0., solver->getInfinity(), -1.);

    //-infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    solver->addRow(row1, -solver->getInfinity(), 3.);

    //-infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    solver->addRow(row2, -solver->getInfinity(), 3.);

    solver->initialSolve();

    solver->writeLp("model");
}

using namespace idol;

std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>
count_variables_and_constraints(const Model& t_model,
                               const Annotation<Var, unsigned int>& t_var_level,
                               const Annotation<Ctr, unsigned int>& t_ctr_level) {

    unsigned int n_lower_level_vars = 0;
    unsigned int n_upper_level_vars = 0;
    unsigned int n_lower_level_ctrs = 0;
    unsigned int n_upper_level_ctrs = 0;

    for (const auto& var : t_model.vars()) {
        if (var.get(t_var_level) != MasterId) {
            n_lower_level_vars++;
        } else {
            n_upper_level_vars++;
        }
    }

    for (const auto& ctr : t_model.ctrs()) {
        if (ctr.get(t_ctr_level) != MasterId) {
            n_lower_level_ctrs++;
        } else {
            n_upper_level_ctrs++;
        }
    }

    return {
        n_lower_level_vars,
        n_upper_level_vars,
        n_lower_level_ctrs,
        n_upper_level_ctrs
    };
}

int main(int t_argc, char** t_argv) {

    SparseMatrix matrix(4);

    matrix.add_major({ 0, 1 }, { 2, 3 });
    matrix.add_major({ 0, 3 }, { 8, 7 });
    matrix.add_major({ 1, 2 }, { .5, .25 });
    matrix.add_major({ 2, 3 }, { 1, 1 });

    std::cout << matrix << std::endl;

    std::cout << matrix.get_coefficient(2, 2) << std::endl;

    return 0;
}
