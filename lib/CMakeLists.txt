add_library(idol STATIC include/modeling/numericals.h include/containers/Optional.h include/containers/Map.h include/modeling.h include/modeling/parameters/Param.h include/modeling/models/Model.h include/modeling/environment/Env.h include/modeling/objects/impl_Object.h src/modeling/objects/impl_Object.cpp src/modeling/models/Model.cpp include/modeling/objects/ObjectId.h src/modeling/objects/ObjectId.cpp include/modeling/objects/Object.h src/modeling/environment/Env.cpp include/modeling/Types.h src/modeling/parameters/Param.cpp include/modeling/environment/ObjectManager.h src/modeling/environment/ObjectManager.cpp include/modeling/expressions/Constant.h src/modeling/matrix/Constant.cpp include/modeling/matrix/AbstractMatrixCoefficient.h include/modeling/matrix/MatrixCoefficientReference.h include/modeling/matrix/MatrixCoefficient.h src/modeling/matrix/MatrixCoefficient.cpp include/modeling/constraints/impl_Constraint.h include/modeling/constraints/Ctr.h include/modeling/variables/impl_Variable.h include/modeling/variables/Var.h src/modeling/constraints/impl_Constraint.cpp src/modeling/variables/impl_Variable.cpp src/modeling/constraints/Constraint.cpp src/modeling/variables/Variable.cpp src/modeling/matrix/MatrixCoefficientReference.cpp include/modeling/constraints/TempCtr.h src/modeling/constraints/TempCtr.cpp include/containers/IteratorForward.h include/modeling/objective/Objective.h include/modeling/models/ListenerManager.h include/modeling/models/Listener.h src/modeling/models/ListenerManager.cpp src/modeling/models/Listener.cpp include/modeling/environment/default_name.h include/modeling/solutions/AbstractSolution.h include/algorithms/solvers/SolutionStatus.h include/solvers.h include/modeling/solutions/Solution.h include/algorithms/branch-and-bound/BranchAndBound.h src/algorithms/branch-and-bound/BranchAndBound.cpp include/algorithms/branch-and-bound/Node.h include/algorithms/Algorithm.h include/algorithms/branch-and-bound/Nodes_Basic.h src/algorithms/branch-and-bound/Nodes_Basic.cpp include/algorithms/logs/Log.h src/algorithms/logs/Log.cpp include/algorithms/decomposition/Decomposition.h include/algorithms/column-generation/ColumnGenerationBranchingSchemes_SP.h src/algorithms/column-generation/ColumnGenerationBranchingSchemes_SP.cpp include/algorithms/column-generation/ColumnGeneration.h include/algorithms/decomposition/GenerationAlgorithmWithAttributes.h src/algorithms/decomposition/GenerationAlgorithm.cpp src/algorithms/column-generation/ColumnGeneration.cpp include/algorithms/column-generation/ColumnGenerationSP.h include/algorithms/decomposition/Generator.h include/modeling/variables/TempVar.h include/containers/Set.h src/algorithms/column-generation/ColumnGenerationSP.cpp include/algorithms/column-generation/ColumnGenerationBranchingScheme.h include/algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.h src/algorithms/column-generation/ColumnGenerationBranchingSchemes_RMP.cpp include/errors/Exception.h src/algorithms/decomposition/Decomposition.cpp include/algorithms/branch-and-bound/NodeStrategy.h include/algorithms/branch-and-bound/NodeStrategies_Basic.h include/algorithms/branch-and-bound/ActiveNodesManager.h include/algorithms/branch-and-bound/BranchingStrategy.h include/algorithms/branch-and-bound/NodeUpdator.h include/algorithms/branch-and-bound/BranchingStrategies_MostInfeasible.h include/algorithms/branch-and-bound/NodeUpdators_ByBoundVar.h include/algorithms.h include/algorithms/branch-and-bound/NodeUpdators_ByBoundCtr.h include/algorithms/attributes/AttributesSchema.h include/algorithms/attributes/AttributesMacros.h include/algorithms/attributes/AttributesSection.h include/algorithms/attributes/Attributes.h include/algorithms/attributes/Attributes_Base.h include/errors/NotImplemented.h include/modeling/expressions/LinExpr.h include/modeling/expressions/Row.h src/modeling/expressions/Row.cpp src/modeling/expressions/Column.cpp include/modeling/expressions/Column.h include/modeling/matrix/Matrix.h src/modeling/matrix/Matrix.cpp include/algorithms/row-generation/RowGeneration.h src/algorithms/row-generation/RowGeneration.cpp include/algorithms/row-generation/RowGenerationSP.h src/algorithms/row-generation/RowGenerationSP.cpp include/algorithms/row-generation/RowGenerationOriginalSpaceBuilder.h include/algorithms/row-generation/RowGenerationOriginalSpaceBuilders_Dual.h src/algorithms/row-generation/RowGenerationOriginalSpaceBuilders_Dual.cpp include/algorithms/row-generation/RowGenerationOriginalSpaceBuilders_Generated.h src/algorithms/row-generation/RowGenerationOriginalSpaceBuilders_Generated.cpp include/algorithms/branch-and-bound/ActiveNodesManagers_Basic.h include/algorithms/attributes/Timer.h src/algorithms/attributes/Timer.cpp include/algorithms/solvers/Solver.h  src/algorithms/solvers/Solvers_GLPK_Simplex.cpp include/algorithms/solvers/Solvers_Gurobi.h src/algorithms/solvers/Solvers_Gurobi.cpp src/problems/GAP/GAP_Instance.cpp include/problems/KP/KP_Instance.h src/problems/KP/KP_Instance.cpp include/problems/MKP/MKP_Instance.h src/problems/MKP/MKP_Instance.cpp include/algorithms/callbacks/Callbacks_RowGeneration.h include/algorithms/callbacks/Callback.h src/algorithms/callbacks/Callback.cpp include/algorithms/callbacks/AlgorithmInCallback.h src/algorithms/callbacks/AlgorithmInCallback.cpp src/algorithms/callbacks/Callbacks_RowGeneration.cpp include/algorithms/row-generation/Attributes_RowGeneration.h src/modeling/objects/Object.cpp include/containers/Vector.h include/algorithms/branch-and-bound/NodeSet.h src/algorithms/callbacks/BranchAndBoundCallback.cpp include/algorithms/callbacks/Callbacks_RoundingHeuristic.h include/algorithms/branch-and-bound/Attributes_BranchAndBound.h include/problems/robust/RobustProblem.h include/modeling/expressions/Expr.h include/modeling/expressions/operators_Var.h src/modeling/expressions/operators_Var.cpp src/modeling/expressions/operators_Ctr.cpp src/modeling/expressions/operators_Constant.cpp include/modeling/expressions/operators_Constant.h include/modeling/expressions/operators_Ctr.h include/modeling/expressions/operators.h include/problems/MKP/MKP_Instance.h src/problems/MKP/MKP_Instance.cpp src/problems/FLP/FLP_Instance.cpp include/problems/FLP/FLP_Instance.h include/utilities/distances.h include/utilities/parse_delimited.h include/algorithms/row-generation/ModelMirror.h src/algorithms/row-generation/ModelMirror.cpp include/modeling/expressions/operators_utils.h include/modeling/expressions/QuadExpr.h include/modeling/expressions/AbstractExpr.h)

#find_package(OpenMP REQUIRED)
#target_link_libraries(idol PUBLIC OpenMP::OpenMP_CXX)

if (${USE_GUROBI})

    if (NOT DEFINED GUROBI_DIR)
        set(GUROBI_DIR $ENV{GUROBI_HOME})
    endif()
    find_package(GUROBI REQUIRED)
    target_link_libraries(idol PUBLIC gurobi)
    target_compile_definitions(idol PUBLIC IDOL_USE_GUROBI)

endif()

if (${USE_GLPK})

    if (NOT DEFINED GLPK_DIR)
        set(GLPK_DIR $ENV{GLPK_HOME})
    endif()
    find_package(GLPK REQUIRED)
    target_link_libraries(idol PUBLIC glpk)
    target_compile_definitions(idol PUBLIC IDOL_USE_GLPK)

endif()

target_include_directories(
        idol
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/include>
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/lib/include/"
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
        "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindGUROBI.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/FindGLPK.cmake"
        DESTINATION lib/cmake/Idol
        )
