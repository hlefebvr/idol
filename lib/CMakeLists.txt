add_library(idol STATIC include/modeling/numericals.h include/containers/Optional.h include/containers/Map.h include/modeling.h include/modeling/input-parameters/InParam.h include/modeling/Types.h include/modeling/expressions/Constant.h src/modeling/expressions/Constant.cpp include/modeling/matrix/AbstractMatrixCoefficient.h include/modeling/matrix/MatrixCoefficientReference.h include/modeling/matrix/MatrixCoefficient.h src/modeling/matrix/MatrixCoefficient.cpp src/modeling/matrix/MatrixCoefficientReference.cpp include/modeling/constraints/TempCtr.h src/modeling/constraints/TempCtr.cpp include/containers/IteratorForward.h include/modeling/solutions/AbstractSolution.h include/modeling/solutions/Solution.h include/modeling/variables/TempVar.h include/containers/Set.h include/errors/NotImplemented.h include/modeling/expressions/LinExpr.h include/modeling/matrix/Row.h src/modeling/matrix/Row.cpp src/modeling/matrix/Column.cpp include/modeling/matrix/Column.h include/modeling/matrix/Matrix.h src/modeling/matrix/Matrix.cpp src/problems/generalized-assignment-problem/GAP_Instance.cpp include/problems/knapsack-problem/KP_Instance.h src/problems/knapsack-problem/KP_Instance.cpp include/problems/multiple-knapsack-problem/MKP_Instance.h src/problems/multiple-knapsack-problem/MKP_Instance.cpp include/modeling/expressions/Expr.h include/modeling/expressions/operations/operators_Var.h src/modeling/expressions/operations/operators_Var.cpp src/modeling/expressions/operations/operators_Ctr.cpp src/modeling/expressions/operations/operators_Constant.cpp include/modeling/expressions/operations/operators_Constant.h include/modeling/expressions/operations/operators_Ctr.h include/modeling/expressions/operations/operators.h include/problems/multiple-knapsack-problem/MKP_Instance.h src/problems/multiple-knapsack-problem/MKP_Instance.cpp src/problems/facility-location-problem/FLP_Instance.cpp include/problems/facility-location-problem/FLP_Instance.h include/problems/helpers/distances.h include/problems/helpers/parse_delimited.h include/modeling/expressions/operations/operators_utils.h include/modeling/expressions/QuadExpr.h include/modeling/expressions/AbstractExpr.h include/modeling/attributes/Attributes.h include/modeling/variables/Attributes_Var.h include/modeling/attributes/AttributeManagers_Base.h include/modeling/constraints/Attributes_Ctr.h include/modeling/models/Attributes_Model.h include/modeling/attributes/AttributeManager.h include/modeling/attributes/AttributeManager_Delegate.h include/errors/UnsupportedRequest.h src/modeling/attributes/AttributeManager.cpp include/modeling/models/Model.h include/modeling/objects/Env.h include/modeling/variables/Var.h src/modeling/objects/Env.cpp include/modeling/variables/VarVersion.h include/modeling/objects/Versions.h include/modeling/objects/Object.h include/modeling/objects/ObjectId.h src/modeling/models/Model.cpp include/modeling/constraints/Ctr.h include/modeling/constraints/CtrVersion.h include/modeling/objects/Version.h src/modeling/variables/Var.cpp src/modeling/constraints/Ctr.cpp include/backends/Backend.h include/backends/solvers/Gurobi.h include/backends/solvers/LazyBackend.h src/backends/solvers/Gurobi.cpp src/backends/Backend.cpp src/backends/parameters/Log.cpp include/backends/branch-and-bound/BranchAndBound.h src/backends/branch-and-bound/BranchAndBound.cpp include/backends/Algorithm.h src/backends/Algorithm.cpp src/backends/branch-and-bound/Nodes_Basic.cpp src/backends/parameters/Timer.cpp include/backends/Relaxation.h include/backends/branch-and-bound/RelaxationManager.h include/backends/branch-and-bound/Relaxations_Continuous.h include/modeling/annotations/Annotation.h src/modeling/annotations/impl_Annotation.cpp include/modeling/annotations/impl_Annotation.h include/backends/BranchAndBoundMIP.h include/backends/column-generation/Relaxations_DantzigWolfe.h src/backends/column-generation/Relaxations_DantzigWolfe.cpp include/backends/column-generation/ColumnGeneration.h src/backends/column-generation/ColumnGeneration.cpp include/backends/BranchAndPriceMIP.h include/modeling/models/BlockModel.h src/modeling/models/BlockModel.cpp include/modeling/models/AbstractModel.h include/backends/column-generation/ColumnGenerationSP.h include/containers/GeneratorPool.h src/backends/column-generation/ColumnGenerationSP.cpp include/modeling/solutions/types.h include/backends/solvers/GLPK.h include/backends/NoAvailableBackend.h include/solvers.h src/backends/solvers/GLPK.cpp include/backends/callback/Callback.h src/backends/callback/Callback.cpp include/backends/column-generation/Callbacks_IntegerMaster.h include/backends/solvers/Mosek.h src/backends/solvers/Mosek.cpp include/linear-algebra/MatrixIndices.h include/linear-algebra/SquareMatrix.h include/linear-algebra/to_rotated_quadratic_cone.h include/problems/robust-problem/RobustProblem.h src/problems/robust-problem/RobustProblem.cpp include/backends/row-generation/RowGeneration.h include/backends/row-generation/RowGenerationSP.h src/backends/row-generation/RowGeneration.cpp src/backends/row-generation/RowGenerationSP.cpp include/backends/branch-and-bound-v2/BranchAndBoundV2.h include/backends/OptimizerFactory.h include/backends/solvers/DefaultOptimizer.h include/backends/branch-and-bound-v2/BranchAndBoundOptimizer.h include/backends/branch-and-bound-v2/relaxations/factories/RelaxationBuilderFactory.h include/backends/branch-and-bound-v2/branching-rules/factories/BranchingRuleFactory.h include/backends/branch-and-bound-v2/node-selection-rules/factories/NodeSelectionRuleFactory.h include/backends/branch-and-bound-v2/relaxations/impls/ContinuousRelaxation.h include/backends/branch-and-bound-v2/branching-rules/factories/MostInfeasible.h include/backends/branch-and-bound-v2/node-selection-rules/factories/DepthFirst.h include/backends/branch-and-bound-v2/branching-rules/impls/BranchingRule.h include/backends/branch-and-bound-v2/node-selection-rules/impls/NodeSelectionRule.h include/backends/branch-and-bound-v2/nodes/Node2.h include/backends/branch-and-bound-v2/nodes/NodeUpdator.h include/backends/branch-and-bound-v2/nodes/NodeUpdatorByBound.h src/backends/branch-and-bound-v2/nodes/NodeInfo.cpp include/backends/branch-and-bound-v2/nodes/NodeInfo.h include/backends/branch-and-bound-v2/branching-rules/impls/MostInfeasbile.h include/backends/branch-and-bound-v2/node-selection-rules/impls/DepthFirst.h include/backends/branch-and-bound-v2/branching-rules/impls/VariableBranchingRule.h include/backends/branch-and-bound-v2/node-selection-rules/factories/BreadthFirst.h include/backends/branch-and-bound-v2/node-selection-rules/factories/WorstBound.h include/backends/branch-and-bound-v2/node-selection-rules/factories/BestBound.h include/backends/branch-and-bound-v2/node-selection-rules/impls/WorstBound.h include/backends/branch-and-bound-v2/node-selection-rules/impls/BestBound.h include/backends/branch-and-bound-v2/node-selection-rules/impls/BreadthFirst.h)

find_package(OpenMP REQUIRED)
target_link_libraries(idol PUBLIC OpenMP::OpenMP_CXX)

if (${USE_ROOT})

    find_package(ROOT REQUIRED)
    target_link_libraries(idol PUBLIC ROOT::Core ROOT::Graf ROOT::Gpad)
    target_compile_definitions(idol PUBLIC IDOL_USE_ROOT)

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

endif()

if (${USE_MOSEK})

    find_package(MOSEK REQUIRED)
    target_link_libraries(idol PUBLIC mosek)
    target_compile_definitions(idol PUBLIC IDOL_USE_MOSEK)

endif()

if (${USE_GLPK})

    find_package(GLPK REQUIRED)
    target_link_libraries(idol PUBLIC glpk)
    target_compile_definitions(idol PUBLIC IDOL_USE_GLPK)

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
        IdolConfigVersion.cmake
        VERSION ${CMAKE_PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

install(EXPORT idol_targets
        FILE IdolTargets.cmake
        DESTINATION lib/cmake/Idol
        )

configure_file(IdolConfig.cmake.in IdolConfig.cmake @ONLY)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/IdolConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/IdolConfigVersion.cmake"
        "${idol_SOURCE_DIR}/cmake/FindGUROBI.cmake"
        "${idol_SOURCE_DIR}/cmake/FindGLPK.cmake"
        "${idol_SOURCE_DIR}/cmake/FindMOSEK.cmake"
        "${idol_SOURCE_DIR}/cmake/FindEIGEN.cmake"
        DESTINATION lib/cmake/Idol
        )
