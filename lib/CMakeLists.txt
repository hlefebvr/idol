add_library(idol STATIC
        include/idol/modeling/numericals.h
        include/idol/containers/Optional.h
        include/idol/containers/Map.h
        include/idol/modeling.h
        include/idol/modeling/parameters/Param.h
        include/idol/modeling/Types.h
        include/idol/modeling/expressions/Constant.h
        src/modeling/expressions/Constant.cpp
        include/idol/modeling/matrix/AbstractMatrixCoefficient.h
        include/idol/modeling/matrix/MatrixCoefficientReference.h
        include/idol/modeling/matrix/MatrixCoefficient.h
        src/modeling/matrix/MatrixCoefficient.cpp
        src/modeling/matrix/MatrixCoefficientReference.cpp
        include/idol/modeling/constraints/TempCtr.h
        src/modeling/constraints/TempCtr.cpp
        include/idol/containers/IteratorForward.h
        include/idol/modeling/solutions/AbstractSolution.h
        include/idol/modeling/solutions/Solution.h
        include/idol/modeling/variables/TempVar.h
        include/idol/containers/Set.h
        include/idol/errors/NotImplemented.h
        include/idol/modeling/expressions/LinExpr.h
        include/idol/modeling/matrix/Row.h
        src/modeling/matrix/Row.cpp
        src/modeling/matrix/Column.cpp
        include/idol/modeling/matrix/Column.h
        include/idol/modeling/matrix/Matrix.h
        src/modeling/matrix/Matrix.cpp
        src/problems/generalized-assignment-problem/GAP_Instance.cpp
        include/idol/problems/knapsack-problem/KP_Instance.h
        src/problems/knapsack-problem/KP_Instance.cpp
        include/idol/problems/multiple-knapsack-problem/MKP_Instance.h
        src/problems/multiple-knapsack-problem/MKP_Instance.cpp
        include/idol/modeling/expressions/Expr.h
        include/idol/modeling/expressions/operations/operators_Var.h
        src/modeling/expressions/operations/operators_Var.cpp
        src/modeling/expressions/operations/operators_Ctr.cpp
        src/modeling/expressions/operations/operators_Constant.cpp
        include/idol/modeling/expressions/operations/operators_Constant.h
        include/idol/modeling/expressions/operations/operators_Ctr.h
        include/idol/modeling/expressions/operations/operators.h
        include/idol/problems/multiple-knapsack-problem/MKP_Instance.h
        src/problems/multiple-knapsack-problem/MKP_Instance.cpp
        src/problems/facility-location-problem/FLP_Instance.cpp
        include/idol/problems/facility-location-problem/FLP_Instance.h
        include/idol/problems/helpers/distances.h
        include/idol/problems/helpers/parse_delimited.h
        include/idol/modeling/expressions/operations/operators_utils.h
        include/idol/modeling/expressions/QuadExpr.h
        include/idol/modeling/expressions/AbstractExpr.h
        include/idol/modeling/models/Model.h
        include/idol/modeling/objects/Env.h
        include/idol/modeling/variables/Var.h
        src/modeling/objects/Env.cpp
        include/idol/modeling/variables/VarVersion.h
        include/idol/modeling/objects/Versions.h
        include/idol/modeling/objects/Object.h
        include/idol/modeling/objects/ObjectId.h
        src/modeling/models/Model.cpp
        include/idol/modeling/constraints/Ctr.h
        include/idol/modeling/constraints/CtrVersion.h
        include/idol/modeling/objects/Version.h
        src/modeling/variables/Var.cpp
        src/modeling/constraints/Ctr.cpp
        include/idol/optimizers/Optimizer.h
        include/idol/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h
        include/idol/optimizers/wrappers/OptimizerWithLazyUpdates.h
        src/optimizers/wrappers/gurobi/Optimizers_Gurobi.cpp
        src/optimizers/Optimizer.cpp
        src/optimizers/Logger.cpp
        include/idol/optimizers/Algorithm.h
        src/optimizers/Algorithm.cpp
        src/optimizers/Timer.cpp
        include/idol/modeling/annotations/Annotation.h
        src/modeling/annotations/impl_Annotation.cpp
        include/idol/modeling/annotations/impl_Annotation.h
        include/idol/modeling/models/Model.h
        include/idol/containers/GeneratorPool.h
        include/idol/modeling/solutions/types.h
        include/idol/optimizers/wrappers/GLPK/Optimizers_GLPK.h
        include/idol/solvers.h
        src/optimizers/wrappers/GLPK/Optimizers_GLPK.cpp
        include/idol/optimizers/wrappers/Mosek/Optimizers_Mosek.h
        src/optimizers/wrappers/Mosek/Optimizers_Mosek.cpp
        include/idol/linear-algebra/MatrixIndices.h
        include/idol/linear-algebra/SquareMatrix.h
        include/idol/linear-algebra/to_rotated_quadratic_cone.h
        include/idol/optimizers/branch-and-bound/Optimizers_BranchAndBound.h
        include/idol/optimizers/OptimizerFactory.h
        include/idol/optimizers/branch-and-bound/BranchAndBound.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/BranchingRuleFactory.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/factories/NodeSelectionRuleFactory.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/factories/DepthFirst.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/BranchingRule.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/impls/NodeSelectionRule.h
        include/idol/optimizers/branch-and-bound/nodes/Node.h
        include/idol/optimizers/branch-and-bound/nodes/NodeUpdator.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/impls/DepthFirst.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/factories/BreadthFirst.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/factories/WorstBound.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/impls/WorstBound.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/impls/BestBound.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/impls/BreadthFirst.h
        include/idol/optimizers/wrappers/Gurobi/Gurobi.h
        include/idol/optimizers/wrappers/GLPK/GLPK.h
        include/idol/optimizers/wrappers/Mosek/Mosek.h
        src/optimizers/wrappers/gurobi/Gurobi.cpp
        src/optimizers/wrappers/GLPK/GLPK.cpp
        src/optimizers/wrappers/Mosek/Mosek.cpp
        include/idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h
        include/idol/optimizers/callbacks/IntegerMaster.h
        src/optimizers/callbacks/IntegerMaster.cpp
        src/optimizers/wrappers/gurobi/GurobiCallbackI.cpp
        include/idol/optimizers/callbacks/Callback.h
        src/optimizers/callbacks/Callback.cpp
        include/idol/optimizers/callbacks/CutSeparation.h
        src/optimizers/callbacks/CutSeparation.cpp
        include/idol/optimizers/callbacks/LazyCutCallback.h
        include/idol/optimizers/callbacks/UserCutCallback.h
        include/idol/optimizers/callbacks/CallbackFactory.h
        include/idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h
        include/idol/optimizers/branch-and-bound/callbacks/AbstractBranchAndBoundCallbackI.h
        include/idol/optimizers/branch-and-bound/callbacks/CallbackAsBranchAndBoundCallback.h
        include/idol/optimizers/branch-and-bound/cutting-planes/CuttingPlaneGenerator.h
        include/idol/optimizers/branch-and-bound/cutting-planes/CoverCuts.h
        include/idol/problems/staff-rostering-problem/SRP_Instance.h
        src/problems/staff-rostering-problem/SRP_Instance.cpp
        src/modeling/numericals.cpp
        include/idol/optimizers/callbacks/SimpleRounding.h
        src/optimizers/callbacks/SimpleRounding.cpp
        src/modeling/expressions/operations/operators.cpp
        include/idol/optimizers/callbacks/RENS.h
        src/optimizers/callbacks/RENS.cpp
        include/idol/optimizers/callbacks/LocalBranching.h
        src/optimizers/callbacks/LocalBranching.cpp
        include/idol/optimizers/branch-and-bound/branching-rules/impls/VariableBranching.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/VariableBranching.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/MostInfeasible.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/LeastInfeasible.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/LeastInfeasible.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/UniformlyRandom.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/UniformlyRandom.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/FirstInfeasibleFound.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/FirstInfeasibleFound.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/StrongBranching.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/NodeScoreFunction.h
        src/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.cpp
        include/idol/optimizers/branch-and-bound/branching-rules/impls/strong-branching/StrongBranchingPhase.h
        src/optimizers/branch-and-bound/branching-rules/factories/StrongBranchingPhase.cpp
        include/idol/optimizers/branch-and-bound/branching-rules/factories/PseudoCost.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/PseudoCost.h
        include/idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h
        include/idol/optimizers/branch-and-bound/nodes/DefaultNodeUpdator.h
        src/optimizers/branch-and-bound/nodes/DefaultNodeInfo.cpp
        include/idol/optimizers/wrappers/HiGHS/HiGHS.h
        src/optimizers/wrappers/HiGHS/HiGHS.cpp
        src/optimizers/wrappers/HiGHS/Optimizers_HiGHS.cpp
        include/idol/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h
        include/idol/optimizers/branch-and-bound/node-selection-rules/impls/BestEstimate.h
        include/idol/optimizers/branch-and-bound/branching-rules/impls/Diver.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/Diver.h
        include/idol/optimizers/branch-and-bound/watchers/ExportBranchAndBoundTreeToCSV.h
        src/optimizers/branch-and-bound/watchers/BranchAndBoundTree.cpp
        include/idol/optimizers/wrappers/MinKnap/MinKnap.h
        include/idol/optimizers/wrappers/MinKnap/Optimizers_MinKnap.h
        src/optimizers/wrappers/MinKnap/MinKnap.cpp
        src/optimizers/wrappers/MinKnap/Optimizers_MinKnap.cpp
        include/idol/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h
        src/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.cpp
        include/idol/optimizers/dantzig-wolfe/SubProblem.h
        src/optimizers/dantzig-wolfe/SubProblem.cpp
        include/idol/optimizers/dantzig-wolfe/Formulation.h
        src/optimizers/dantzig-wolfe/Formulation.cpp
        include/idol/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.h
        src/optimizers/dantzig-wolfe/Optimizers_DantzigWolfeDecomposition.cpp
        include/idol/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.h
        include/idol/optimizers/dantzig-wolfe/ColumnGeneration.h
        src/optimizers/dantzig-wolfe/ColumnGeneration.cpp
        include/idol/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.h
        src/optimizers/dantzig-wolfe/infeasibility-strategies/FarkasPricing.cpp
        src/optimizers/dantzig-wolfe/infeasibility-strategies/DantzigWolfeInfeasibilityStrategy.cpp
        include/idol/optimizers/dantzig-wolfe/stabilization/DualPriceSmoothingStabilization.h
        include/idol/optimizers/dantzig-wolfe/stabilization/Neame.h
        include/idol/optimizers/dantzig-wolfe/stabilization/Wentges.h
        include/idol/optimizers/dantzig-wolfe/stabilization/NoStabilization.h
        include/idol/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.h
        src/optimizers/dantzig-wolfe/infeasibility-strategies/ArtificialCosts.cpp
        include/idol/optimizers/dantzig-wolfe/logs/LoggerFactory.h
        include/idol/optimizers/dantzig-wolfe/logs/Debug.h
        src/optimizers/dantzig-wolfe/logs/Debug.cpp
        include/idol/optimizers/logs.h
        include/idol/optimizers/branch-and-bound/branching-rules/factories/BracnhingWithPriority.h
        src/optimizers/logs.cpp
        include/idol/optimizers/branch-and-bound/branching-rules/impls/BranchingWithPriority.h
        include/idol/optimizers/branch-and-bound/nodes/BranchingDecision.h
        include/idol/containers/Pair.h
)

find_package(OpenMP REQUIRED)
target_link_libraries(idol PUBLIC OpenMP::OpenMP_CXX)

if (${USE_ROBINHOOD})

    find_package(ROBINHOOD REQUIRED)
    target_link_libraries(idol PUBLIC robin_hood)
    target_compile_definitions(idol PUBLIC IDOL_USE_ROBINHOOD)

endif()

if (${USE_EIGEN})

    find_package(EIGEN REQUIRED)
    target_link_libraries(idol PUBLIC eigen)
    target_compile_definitions(idol PUBLIC IDOL_USE_EIGEN)

endif()

if (${USE_GUROBI})

    find_package(GUROBI REQUIRED)
    target_link_libraries(idol PUBLIC gurobi)
    target_compile_definitions(idol PUBLIC IDOL_USE_GUROBI)
    list(APPEND AVAILABLE_MILP_SOLVERS "Gurobi")

endif()

if (${USE_MOSEK})

    find_package(MOSEK REQUIRED)
    target_link_libraries(idol PUBLIC mosek)
    target_compile_definitions(idol PUBLIC IDOL_USE_MOSEK)
    list(APPEND AVAILABLE_MILP_SOLVERS "Mosek")

endif()

if (${USE_GLPK})

    find_package(GLPK REQUIRED)
    target_link_libraries(idol PUBLIC glpk)
    target_compile_definitions(idol PUBLIC IDOL_USE_GLPK)
    list(APPEND AVAILABLE_MILP_SOLVERS "GLPK")

endif()

if (${USE_HIGHS})

    find_package(Threads REQUIRED)
    find_package(HIGHS REQUIRED)
    target_link_libraries(idol PUBLIC highs::highs)
    target_compile_definitions(idol PUBLIC IDOL_USE_HIGHS)
    list(APPEND AVAILABLE_MILP_SOLVERS "HiGHS")

endif()

if (${USE_MINKNAP})

    find_package(minknap REQUIRED)
    target_link_libraries(idol PUBLIC minknap)
    target_compile_definitions(idol PUBLIC IDOL_USE_MINKNAP)

endif()

set(AVAILABLE_MILP_SOLVERS "${AVAILABLE_MILP_SOLVERS}" PARENT_SCOPE)

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
        PATTERN "*.h"
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
