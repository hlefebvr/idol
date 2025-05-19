add_library(idol STATIC
        include/idol/general/numericals.h
        include/idol/general/utils/Optional.h
        include/idol/general/utils/Map.h
        include/idol/modeling.h
        include/idol/mixed-integer/modeling/parameters/Param.h
        include/idol/mixed-integer/modeling/Types.h
        include/idol/mixed-integer/modeling/constraints/TempCtr.h
        src/mixed-integer/modeling/constraints/TempCtr.cpp
        include/idol/general/utils/IteratorForward.h
        include/idol/mixed-integer/modeling/variables/TempVar.h
        include/idol/general/utils/Set.h
        include/idol/general/utils/exceptions/NotImplemented.h
        include/idol/mixed-integer/modeling/expressions/LinExpr.h
        src/mixed-integer/problems/generalized-assignment-problem/GAP_Instance.cpp
        include/idol/mixed-integer/problems/knapsack-problem/KP_Instance.h
        src/mixed-integer/problems/knapsack-problem/KP_Instance.cpp
        include/idol/mixed-integer/problems/multiple-knapsack-problem/MKP_Instance.h
        src/mixed-integer/problems/multiple-knapsack-problem/MKP_Instance.cpp
        include/idol/mixed-integer/modeling/expressions/AffExpr.h
        include/idol/mixed-integer/modeling/expressions/operations/operators.h
        include/idol/mixed-integer/problems/multiple-knapsack-problem/MKP_Instance.h
        src/mixed-integer/problems/multiple-knapsack-problem/MKP_Instance.cpp
        src/mixed-integer/problems/facility-location-problem/FLP_Instance.cpp
        include/idol/mixed-integer/problems/facility-location-problem/FLP_Instance.h
        include/idol/mixed-integer/problems/helpers/distances.h
        include/idol/mixed-integer/problems/helpers/parse_delimited.h
        include/idol/mixed-integer/modeling/expressions/operations/operators_utils.h
        include/idol/mixed-integer/modeling/models/Model.h
        include/idol/mixed-integer/modeling/objects/Env.h
        include/idol/mixed-integer/modeling/variables/Var.h
        src/mixed-integer/modeling/objects/Env.cpp
        include/idol/mixed-integer/modeling/variables/VarVersion.h
        include/idol/mixed-integer/modeling/objects/Versions.h
        include/idol/mixed-integer/modeling/objects/Object.h
        include/idol/mixed-integer/modeling/objects/ObjectId.h
        src/mixed-integer/modeling/models/Model.cpp
        include/idol/mixed-integer/modeling/constraints/Ctr.h
        include/idol/mixed-integer/modeling/constraints/CtrVersion.h
        include/idol/mixed-integer/modeling/objects/Version.h
        src/mixed-integer/modeling/variables/Var.cpp
        src/mixed-integer/modeling/constraints/Ctr.cpp
        include/idol/general/optimizers/Optimizer.h
        include/idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h
        include/idol/general/optimizers/OptimizerWithLazyUpdates.h
        src/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.cpp
        src/general/optimizers/Optimizer.cpp
        include/idol/general/optimizers/Algorithm.h
        src/general/optimizers/Algorithm.cpp
        src/general/optimizers/Timer.cpp
        include/idol/mixed-integer/modeling/annotations/Annotation.h
        src/mixed-integer/modeling/annotations/impl_Annotation.cpp
        include/idol/mixed-integer/modeling/annotations/impl_Annotation.h
        include/idol/mixed-integer/modeling/models/Model.h
        include/idol/general/utils/GeneratorPool.h
        include/idol/general/utils/types.h
        include/idol/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.h
        include/idol/solvers.h
        src/mixed-integer/optimizers/wrappers/GLPK/Optimizers_GLPK.cpp
        include/idol/mixed-integer/optimizers/wrappers/Mosek/Optimizers_Mosek.h
        src/mixed-integer/optimizers/wrappers/Mosek/Optimizers_Mosek.cpp
        include/idol/general/linear-algebra/MatrixIndices.h
        include/idol/general/linear-algebra/SquareMatrix.h
        include/idol/general/linear-algebra/to_rotated_quadratic_cone.h
        include/idol/mixed-integer/optimizers/branch-and-bound/Optimizers_BranchAndBound.h
        include/idol/general/optimizers/OptimizerFactory.h
        include/idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/NodeSelectionRuleFactory.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/BranchingRule.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/NodeSelectionRule.h
        include/idol/mixed-integer/optimizers/branch-and-bound/nodes/Node.h
        include/idol/mixed-integer/optimizers/branch-and-bound/nodes/NodeUpdator.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/DepthFirst.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/WorstBound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/BestBound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/BreadthFirst.h
        include/idol/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.h
        include/idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h
        include/idol/mixed-integer/optimizers/wrappers/Mosek/Mosek.h
        src/mixed-integer/optimizers/wrappers/Gurobi/Gurobi.cpp
        src/mixed-integer/optimizers/wrappers/GLPK/GLPK.cpp
        src/mixed-integer/optimizers/wrappers/Mosek/Mosek.cpp
        include/idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h
        include/idol/mixed-integer/optimizers/callbacks/heuristics/IntegerMaster.h
        src/mixed-integer/optimizers/wrappers/Gurobi/GurobiCallbackI.cpp
        include/idol/mixed-integer/optimizers/callbacks/Callback.h
        src/mixed-integer/optimizers/callbacks/Callback.cpp
        include/idol/mixed-integer/optimizers/callbacks/cutting-planes/CutSeparation.h
        src/mixed-integer/optimizers/callbacks/CutSeparation.cpp
        include/idol/mixed-integer/optimizers/callbacks/cutting-planes/LazyCutCallback.h
        include/idol/mixed-integer/optimizers/callbacks/cutting-planes/UserCutCallback.h
        include/idol/mixed-integer/optimizers/callbacks/CallbackFactory.h
        include/idol/mixed-integer/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h
        include/idol/mixed-integer/optimizers/branch-and-bound/callbacks/AbstractBranchAndBoundCallbackI.h
        include/idol/mixed-integer/optimizers/branch-and-bound/callbacks/CallbackAsBranchAndBoundCallback.h
        include/idol/mixed-integer/problems/staff-rostering-problem/SRP_Instance.h
        src/mixed-integer/problems/staff-rostering-problem/SRP_Instance.cpp
        include/idol/mixed-integer/optimizers/callbacks/heuristics/SimpleRounding.h
        src/mixed-integer/optimizers/callbacks/heuristics/SimpleRounding.cpp
        include/idol/mixed-integer/optimizers/callbacks/heuristics/RENS.h
        src/mixed-integer/optimizers/callbacks/heuristics/RENS.cpp
        include/idol/mixed-integer/optimizers/callbacks/heuristics/LocalBranching.h
        src/mixed-integer/optimizers/callbacks/heuristics/LocalBranching.cpp
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/VariableBranching.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/VariableBranching.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/MostInfeasible.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/LeastInfeasible.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/LeastInfeasible.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/UniformlyRandom.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/UniformlyRandom.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/FirstInfeasibleFound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/FirstInfeasibleFound.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/StrongBranching.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/NodeScoreFunction.h
        src/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.cpp
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h
        src/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranchingPhase.cpp
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/PseudoCost.h
        include/idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h
        include/idol/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeUpdator.h
        src/mixed-integer/optimizers/branch-and-bound/nodes/DefaultNodeInfo.cpp
        include/idol/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.h
        src/mixed-integer/optimizers/wrappers/HiGHS/HiGHS.cpp
        src/mixed-integer/optimizers/wrappers/HiGHS/Optimizers_HiGHS.cpp
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h
        include/idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/impls/BestEstimate.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/Diver.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/Diver.h
        include/idol/mixed-integer/optimizers/callbacks/watchers/ExportBranchAndBoundTreeToCSV.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h
        src/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/SubProblem.h
        src/mixed-integer/optimizers/dantzig-wolfe/SubProblem.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/Formulation.h
        src/mixed-integer/optimizers/dantzig-wolfe/Formulation.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h
        src/mixed-integer/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.h
        src/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h
        src/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.cpp
        src/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/DualPriceSmoothingStabilization.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Neame.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/Wentges.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/stabilization/NoStabilization.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h
        src/mixed-integer/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.cpp
        include/idol/mixed-integer/optimizers/dantzig-wolfe/logs/Factory.h
        include/idol/mixed-integer/optimizers/dantzig-wolfe/logs/Info.h
        src/mixed-integer/optimizers/dantzig-wolfe/logs/Info.cpp
        include/idol/general/optimizers/logs.h
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/BranchingWithPriority.h
        src/general/optimizers/logs.cpp
        include/idol/mixed-integer/optimizers/branch-and-bound/branching-rules/impls/BranchingWithPriority.h
        include/idol/mixed-integer/optimizers/branch-and-bound/nodes/BranchingDecision.h
        include/idol/general/utils/Pair.h
        include/idol/mixed-integer/optimizers/callbacks/cutting-planes/KnapsackCover.h
        include/idol/mixed-integer/optimizers/branch-and-bound/logs/Factory.h
        include/idol/mixed-integer/optimizers/branch-and-bound/logs/Info.h
        include/idol/general/utils/Finally.h
        src/bilevel/optimizers/wrappers/MibS/MibS.cpp
        include/idol/bilevel/optimizers/wrappers/MibS/MibS.h
        src/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.cpp
        include/idol/bilevel/optimizers/wrappers/MibS/Optimizers_MibS.h
        src/bilevel/optimizers/wrappers/MibS/impl_MibSFromAPI.cpp
        include/idol/bilevel/optimizers/wrappers/MibS/impl_MibSFromAPI.h
        src/mixed-integer/optimizers/wrappers/Osi/OsiIdolSolverInterface.cpp
        include/idol/mixed-integer/optimizers/wrappers/Osi/OsiIdolSolverInterface.h
        src/mixed-integer/optimizers/wrappers/Osi/Osi.cpp
        include/idol/mixed-integer/optimizers/wrappers/Osi/Osi.h
        src/mixed-integer/optimizers/wrappers/Osi/Optimizers_Osi.cpp
        include/idol/mixed-integer/optimizers/wrappers/Osi/Optimizers_Osi.h
        include/idol/bilevel/modeling/read_from_file.h
        src/bilevel/modeling/read_from_file.cpp
        include/idol/mixed-integer/optimizers/wrappers/Mosek/MosekCallbackI.h
        src/mixed-integer/optimizers/wrappers/Mosek/MosekCallbackI.cpp
        include/idol/bilevel/modeling/Description.h
        include/idol/bilevel/modeling/write_to_file.h
        src/bilevel/modeling/write_to_file.cpp
        include/idol/general/utils/LimitedWidthStream.h
        include/idol/bilevel/optimizers/wrappers/MibS/impl_MibSFromFile.h
        src/bilevel/optimizers/wrappers/MibS/impl_MibSFromFile.cpp
        include/idol/general/utils/SilentMode.h
        src/mixed-integer/optimizers/padm/PADM.cpp
        include/idol/mixed-integer/optimizers/padm/PADM.h
        src/mixed-integer/optimizers/padm/Optimizers_PADM.cpp
        include/idol/mixed-integer/optimizers/padm/Optimizers_PADM.h
        src/mixed-integer/optimizers/padm/Formulation.cpp
        include/idol/mixed-integer/optimizers/padm/Formulation.h
        src/mixed-integer/optimizers/padm/SubProblem.cpp
        include/idol/mixed-integer/optimizers/padm/SubProblem.h
        src/mixed-integer/optimizers/padm/PenaltyUpdates.cpp
        include/idol/mixed-integer/optimizers/padm/PenaltyUpdates.h
        src/mixed-integer/optimizers/padm/PenaltyMethod.cpp
        include/idol/mixed-integer/optimizers/padm/PenaltyMethod.h
        include/idol/bilevel/optimizers/wrappers/MibS/impl_MibS.h
        include/idol/general/utils/uuid.h
        src/general/utils/uuid.cpp
        include/idol/bilevel/optimizers/wrappers/MibS/MibSCallbackI.h
        include/idol/mixed-integer/optimizers/callbacks/watchers/PlotManager.h
        src/mixed-integer/optimizers/callbacks/watchers/PlotManager.cpp
        include/idol/mixed-integer/optimizers/callbacks/watchers/Plots_OptimalityGap.h
        src/mixed-integer/optimizers/callbacks/watchers/Plots_OptimalityGap.cpp
        include/idol/general/utils/SparseVector.h
        include/idol/general/utils/sort.h
        include/idol/general/utils/Point.h
        src/mixed-integer/modeling/variables/VarVersion.cpp
        src/mixed-integer/modeling/constraints/CtrVersion.cpp
        include/idol/mixed-integer/modeling/expressions/QuadExpr.h
        include/idol/mixed-integer/modeling/constraints/QCtr.h
        include/idol/mixed-integer/modeling/constraints/QCtrVersion.h
        include/idol/mixed-integer/modeling/constraints/TempQCtr.h
        src/mixed-integer/modeling/constraints/TempQCtr.cpp
        src/mixed-integer/modeling/constraints/QCtrVersion.cpp
        src/mixed-integer/modeling/constraints/QCtr.cpp
        include/idol/general/utils/GenerationPattern.h
        include/idol/mixed-integer/modeling/models/KKT.h
        src/mixed-integer/modeling/models/KKT.cpp
        include/idol/mixed-integer/optimizers/callbacks/ReducedCostFixing.h
        include/idol/robust/modeling/Description.h
        src/robust/modeling/Description.cpp
        include/idol/robust/optimizers/deterministic/Deterministic.h
        src/robust/optimizers/deterministic/Deterministic.cpp
        include/idol/robust/optimizers/deterministic/Optimizers_Deterministic.h
        src/robust/optimizers/deterministic/Optimizers_Deterministic.cpp
        include/idol/robust/optimizers/affine-decision-rule/AffineDecisionRule.h
        include/idol/robust/optimizers/affine-decision-rule/Optimizers_AffineDecisionRule.h
        src/robust/optimizers/affine-decision-rule/AffineDecisionRule.cpp
        src/robust/optimizers/affine-decision-rule/Optimizers_AffineDecisionRule.cpp
        include/idol/bilevel/optimizers/KKT/KKT.h
        include/idol/bilevel/optimizers/KKT/Optimizers_KKT.h
        src/bilevel/optimizers/KKT/KKT.cpp
        src/bilevel/optimizers/StrongDuality/StrongDuality.cpp
        src/bilevel/optimizers/KKT/Optimizers_KKT.cpp
        src/bilevel/optimizers/StrongDuality/Optimizers_StrongDuality.cpp
        include/idol/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.h
        src/robust/optimizers/column-and-constraint-generation/ColumnAndConstraintGeneration.cpp
        include/idol/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.h
        src/robust/optimizers/column-and-constraint-generation/Optimizers_ColumnAndConstraintGeneration.cpp
        include/idol/robust/optimizers/column-and-constraint-generation/Formulation.h
        src/robust/optimizers/column-and-constraint-generation/Formulation.cpp
        include/idol/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.h
        src/bilevel/optimizers/PessimisticAsOptimistic/PessimisticAsOptimistic.cpp
        include/idol/bilevel/optimizers/BilevelOptimizerInterface.h
        src/bilevel/optimizers/PessimisticAsOptimistic/Optimizers_PessimisticAsOptimistic.cpp
        include/idol/bilevel/optimizers/PessimisticAsOptimistic/Optimizers_PessimisticAsOptimistic.h
        include/idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h
        src/mixed-integer/optimizers/wrappers/Cplex/Cplex.cpp
        include/idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h
        src/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.cpp
        include/idol/mixed-integer/optimizers/wrappers/Cplex/CplexCallbackI.h
        src/mixed-integer/optimizers/wrappers/Cplex/CplexCallbackI.cpp
        include/idol/bilevel/optimizers/BranchAndCut/BranchAndCut.h
        include/idol/bilevel/optimizers/BranchAndCut/Optimizers_BranchAndCut.h
        src/bilevel/optimizers/BranchAndCut/BranchAndCut.cpp
        src/bilevel/optimizers/BranchAndCut/Optimizers_BranchAndCut.cpp
        include/idol/robust/optimizers/KAdaptabilityBranchAndBound/ScenarioBasedKAdaptabilityProblemSolver.h
        src/optimizers/robust/KAdaptabilityBranchAndBound/KAdaptabilityBranchAndBound.cpp
        include/idol/robust/optimizers/KAdaptabilityBranchAndBound/Optimizers_ScenarioBasedKAdaptabilityProblemSolver.h
        src/optimizers/robust/KAdaptabilityBranchAndBound/Optimizers_ScenarioBasedKAdaptabilityProblemSolver.cpp
        src/optimizers/robust/KAdaptabilityBranchAndBound/ScenarioBasedKAdaptabilityProblemSolver.cpp
        include/idol/mixed-integer/modeling/constraints/SOSCtr.h
        include/idol/mixed-integer/modeling/constraints/SOSCtrVersion.h
        src/mixed-integer/modeling/constraints/SOSCtr.cpp
        include/idol/bilevel/optimizers/MinMaxDualize/MinMax_Dualize.h
        include/idol/bilevel/optimizers/MinMaxDualize/Optimizers_MinMax_Dualize.h
        src/bilevel/optimizers/MinMaxDualize/MinMax_Dualize.cpp
        src/bilevel/optimizers/MinMaxDualize/Optimizers_MinMax_Dualize.cpp
        include/idol/robust/optimizers/benders/Benders.h
        include/idol/robust/optimizers/benders/Optimizers_Benders.h
        src/robust/optimizers/benders/Benders.cpp
        src/robust/optimizers/benders/Optimizers_Benders.cpp
        include/idol/robust/optimizers/column-and-constraint-generation/AdversarialProblemSolver.h
)

find_package(OpenMP REQUIRED)
target_link_libraries(idol PUBLIC OpenMP::OpenMP_CXX)

function(set_DIR_variable PREFIX)
    string(TOUPPER "${PREFIX}" PREFIX_UPPER)
    set(VAR_NAME "${PREFIX_UPPER}_DIR")
    set(ENV_NAME "${PREFIX_UPPER}_HOME")

    if(NOT DEFINED ${VAR_NAME} OR "${${VAR_NAME}}" STREQUAL "")
        set(${VAR_NAME} "$ENV{${ENV_NAME}}" CACHE STRING "Path to ${PREFIX}")
    else()
        set(${VAR_NAME} "${${VAR_NAME}}" CACHE STRING "Path to ${PREFIX}")
    endif()
endfunction()


set(USE_ROOT OFF CACHE BOOL "Build idol with ROOT for plotting capabilities (https://root.cern/)")
if (${USE_ROOT})

    find_package(ROOT REQUIRED)
    target_link_libraries(idol PUBLIC ROOT::Core ROOT::Gpad ROOT::Graf)
    target_compile_definitions(idol PUBLIC IDOL_USE_ROOT)
    unset(ROOT_DIR)
    unset(ROOT_rootdraw_CMD)
    unset(ROOT_genmap_CMD)

endif()

set(USE_TSL OFF CACHE BOOL "Build idol with TSL for hash maps (https://github.com/Tessil/ordered-map and https://github.com/Tessil/sparse-map)")
set_DIR_variable(tsl_sparse_map)
set_DIR_variable(tsl_oredered_map)
if (${USE_TSL})

    find_package(TSL REQUIRED)
    target_link_libraries(idol PUBLIC tsl)
    target_compile_definitions(idol PUBLIC IDOL_USE_TSL)

endif ()

set(USE_ROBINHOOD OFF CACHE BOOL "Build idol with Robinhood for hash maps (https://github.com/martinus/robin-hood-hashing)")
set_DIR_variable(RobinHood)
if (${USE_ROBINHOOD})

    find_package(ROBINHOOD REQUIRED)
    target_link_libraries(idol PUBLIC robin_hood)
    target_compile_definitions(idol PUBLIC IDOL_USE_ROBINHOOD)

endif()

set(USE_EIGEN OFF CACHE BOOL "Build idol with Eigen for linear algebra (https://eigen.tuxfamily.org)")
set(EIGEN_DIR "${EIGEN_DIR}" CACHE STRING "Path to Eigen")
if (NOT EIGEN_DIR)
    set(EIGEN_DIR $ENV{EIGEN_HOME})
endif()
if (${USE_EIGEN})

    find_package(EIGEN REQUIRED)
    target_link_libraries(idol PUBLIC eigen)
    target_compile_definitions(idol PUBLIC IDOL_USE_EIGEN)

endif()

set(USE_GUROBI OFF CACHE BOOL "Build idol with Gurobi (https://www.gurobi.com)")
set_DIR_variable(Gurobi)
if (${USE_GUROBI})

    find_package(GUROBI REQUIRED)
    target_link_libraries(idol PUBLIC gurobi)
    target_compile_definitions(idol PUBLIC IDOL_USE_GUROBI)
    list(APPEND AVAILABLE_MILP_SOLVERS "Gurobi")

endif()

set(USE_MOSEK OFF CACHE BOOL "Build idol with Mosek (https://www.mosek.com)")
set_DIR_variable(Mosek)
if (${USE_MOSEK})

    find_package(MOSEK REQUIRED)
    target_link_libraries(idol PUBLIC mosek)
    target_compile_definitions(idol PUBLIC IDOL_USE_MOSEK)
    list(APPEND AVAILABLE_MILP_SOLVERS "Mosek")

endif()

set(USE_GLPK OFF CACHE BOOL "Build idol with GLPK (https://www.gnu.org/software/glpk/)")
set_DIR_variable(GLPK)
if (${USE_GLPK})

    find_package(GLPK REQUIRED)
    target_link_libraries(idol PUBLIC glpk)
    target_compile_definitions(idol PUBLIC IDOL_USE_GLPK)
    list(APPEND AVAILABLE_MILP_SOLVERS "GLPK")

endif()

set(USE_HIGHS OFF CACHE BOOL "Build idol with HiGHS (https://highs.dev/)")
if (${USE_HIGHS})

    find_package(Threads REQUIRED)
    find_package(HIGHS REQUIRED)
    target_link_libraries(idol PUBLIC highs::highs)
    target_compile_definitions(idol PUBLIC IDOL_USE_HIGHS)
    list(APPEND AVAILABLE_MILP_SOLVERS "HiGHS")

endif()

set(USE_CPLEX OFF CACHE BOOL "Build idol with CPLEX (https://www.ibm.com/analytics/cplex-optimizer)")
set_DIR_variable(CPLEX)
if ("${CPLEX_DIR}" STREQUAL "")
    file(GLOB CPLEX_IN_OPT "/opt/ibm/ILOG/CPLEX_Studio*")
    if(CPLEX_IN_OPT_LEN GREATER 0)
        list(GET CPLEX_IN_OPT 0 CPLEX_IN_OPT)
    endif ()
    set(CPLEX_DIR "${CPLEX_IN_OPT}" CACHE STRING "Path to CPLEX" FORCE)
endif()
if (${USE_CPLEX})

    find_package(CPLEX REQUIRED)
    target_link_libraries(idol PUBLIC cplex)
    target_compile_definitions(idol PUBLIC IDOL_USE_CPLEX)
    list(APPEND AVAILABLE_MILP_SOLVERS "Cplex")

endif()

set(USE_OSI_CPLEX OFF CACHE BOOL "Build idol with OsiCplex (https://projects.coin-or.org/OsiCplex)")
if (${USE_OSI_CPLEX})

    set(USE_OSI ON)
    target_compile_definitions(idol PUBLIC IDOL_USE_OSI_CPLEX)
    list(APPEND AVAILABLE_MILP_SOLVERS "OsiCplex")

endif()

set_DIR_variable(coin_or)

set(USE_OSI_SYMPHONY OFF CACHE BOOL "Build idol with SYMPHONY (https://projects.coin-or.org/SYMPHONY)")
if (${USE_OSI_SYMPHONY})

    find_package(SYMPHONY REQUIRED)
    set(USE_OSI ON)
    target_link_libraries(idol PUBLIC symphony)
    target_compile_definitions(idol PUBLIC IDOL_USE_OSI_SYMPHONY)
    list(APPEND AVAILABLE_MILP_SOLVERS "OsiSymphony")

endif()

set(USE_MIBS OFF CACHE BOOL "Build idol with MibS (https://github.com/coin-or/MibS)")
if (${USE_MIBS})

    find_package(ALPS REQUIRED)
    find_package(BCPS REQUIRED)
    find_package(BLIS REQUIRED)
    find_package(CGL REQUIRED)
    set(USE_OSI_CLP ON)
    set(USE_OSI ON)
    target_link_libraries(idol PUBLIC alps bcps blis cgl)

    find_package(MIBS REQUIRED)
    target_link_libraries(idol PUBLIC mibs)
    target_compile_definitions(idol PUBLIC IDOL_USE_MIBS)

    list(APPEND AVAILABLE_BILEVEL_SOLVERS "MibS")

endif()

set(USE_OSI_CLP OFF CACHE BOOL "Build idol with OsiClp (https://projects.coin-or.org/OsiClp)")
if (${USE_OSI_CLP})

    find_package(CLP REQUIRED)
    set(USE_OSI ON)
    target_link_libraries(idol PUBLIC clp)
    target_compile_definitions(idol PUBLIC IDOL_USE_OSI_CLP)
    list(APPEND AVAILABLE_MILP_SOLVERS "OsiClp")

endif()

set(USE_OSI OFF CACHE BOOL "Build idol with Osi (https://projects.coin-or.org/Osi)")
if (${USE_OSI})

    find_package(COINUTILS REQUIRED)
    find_package(OSI REQUIRED)

    target_link_libraries(idol PUBLIC osi coinUtils)
    target_compile_definitions(idol PUBLIC IDOL_USE_OSI)

endif()

set(USE_JULIA OFF CACHE BOOL "Build idol with Julia (https://julialang.org)")
set_DIR_variable(Julia)
if (${USE_JULIA})

    find_package(Julia REQUIRED)
    target_link_libraries(idol PUBLIC julia)

    add_custom_command(TARGET idol POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/include/idol/mixed-integer/optimizers/wrappers/JuMP/module
            $<TARGET_FILE_DIR:idol>/julia
    )

endif()

set(AVAILABLE_MILP_SOLVERS "${AVAILABLE_MILP_SOLVERS}" PARENT_SCOPE)
set(AVAILABLE_BILEVEL_SOLVERS "${AVAILABLE_BILEVEL_SOLVERS}" PARENT_SCOPE)

if (${WITH_PROFILING})
    target_compile_options(idol PRIVATE -pg)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CPP_COMPILER g++)
endif()

target_include_directories(
        idol
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${idol_SOURCE_DIR}/lib/include>
)

install(DIRECTORY "${idol_SOURCE_DIR}/lib/include/"
        DESTINATION include/idol
        FILES_MATCHING
        PATTERN "*.h *.jl"
)

install(TARGETS idol
        EXPORT idol_targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include/idol
        PUBLIC_HEADER DESTINATION include/idol
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        idolConfigVersion.cmake
        VERSION ${CMAKE_PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(EXPORT idol_targets
        FILE idolTargets.cmake
        DESTINATION lib/cmake/idol
        )

configure_file(idolConfig.cmake.in idolConfig.cmake @ONLY)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/idolConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/idolConfigVersion.cmake"
        "${idol_SOURCE_DIR}/cmake/FindGUROBI.cmake"
        "${idol_SOURCE_DIR}/cmake/FindGLPK.cmake"
        "${idol_SOURCE_DIR}/cmake/FindMOSEK.cmake"
        "${idol_SOURCE_DIR}/cmake/FindEIGEN.cmake"
        "${idol_SOURCE_DIR}/cmake/FindROBINHOOD.cmake"
        DESTINATION lib/cmake/idol
        )
