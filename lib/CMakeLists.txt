add_library(idol STATIC include/modeling/numericals.h include/containers/Optional.h include/containers/Map.h include/modeling.h include/modeling/parameters/InParam.h include/modeling/Types.h include/modeling/expressions/Constant.h src/modeling/expressions/Constant.cpp include/modeling/matrix/AbstractMatrixCoefficient.h include/modeling/matrix/MatrixCoefficientReference.h include/modeling/matrix/MatrixCoefficient.h src/modeling/matrix/MatrixCoefficient.cpp src/modeling/matrix/MatrixCoefficientReference.cpp include/modeling/constraints/TempCtr.h src/modeling/constraints/TempCtr.cpp include/containers/IteratorForward.h include/modeling/solutions/AbstractSolution.h include/modeling/solutions/Solution.h include/modeling/variables/TempVar.h include/containers/Set.h include/errors/NotImplemented.h include/modeling/expressions/LinExpr.h include/modeling/matrix/Row.h src/modeling/matrix/Row.cpp src/modeling/matrix/Column.cpp include/modeling/matrix/Column.h include/modeling/matrix/Matrix.h src/modeling/matrix/Matrix.cpp src/problems/GAP/GAP_Instance.cpp include/problems/KP/KP_Instance.h src/problems/KP/KP_Instance.cpp include/problems/MKP/MKP_Instance.h src/problems/MKP/MKP_Instance.cpp include/problems/robust/RobustProblem.h include/modeling/expressions/Expr.h include/modeling/expressions/operations/operators_Var.h src/modeling/expressions/operations/operators_Var.cpp src/modeling/expressions/operations/operators_Ctr.cpp src/modeling/expressions/operations/operators_Constant.cpp include/modeling/expressions/operations/operators_Constant.h include/modeling/expressions/operations/operators_Ctr.h include/modeling/expressions/operations/operators.h include/problems/MKP/MKP_Instance.h src/problems/MKP/MKP_Instance.cpp src/problems/FLP/FLP_Instance.cpp include/problems/FLP/FLP_Instance.h include/utilities/distances.h include/utilities/parse_delimited.h include/modeling/expressions/operations/operators_utils.h include/modeling/expressions/QuadExpr.h include/modeling/expressions/AbstractExpr.h include/modeling/attributes/Attributes.h include/modeling/variables/Attributes_Var.h include/modeling/attributes/AttributeManagers_Base.h include/modeling/constraints/Attributes_Ctr.h include/modeling/models/Attributes_Model.h include/modeling/attributes/AttributeManager.h include/modeling/attributes/AttributeManager_Delegate.h include/errors/UnsupportedRequest.h src/modeling/attributes/AttributeManager.cpp include/modeling/models/Model.h include/modeling/models/Env.h include/modeling/variables/Var.h src/modeling/models/Env.cpp include/modeling/variables/VarVersion.h include/modeling/objects/Versions.h include/modeling/objects/Object.h include/modeling/objects/ObjectId.h src/modeling/models/Model.cpp include/modeling/constraints/Ctr.h include/modeling/constraints/CtrVersion.h include/modeling/objects/Version.h src/modeling/variables/Var.cpp src/modeling/constraints/Ctr.cpp include/backends/Backend.h include/backends/solvers/Gurobi.h include/backends/solvers/LazyBackend.h src/backends/solvers/Gurobi.cpp src/backends/Backend.cpp src/backends/parameters/Log.cpp include/backends/branch-and-bound/BranchAndBound.h src/backends/branch-and-bound/BranchAndBound.cpp include/backends/Algorithm.h src/backends/Algorithm.cpp src/backends/branch-and-bound/Nodes_Basic.cpp src/backends/parameters/Timer.cpp include/backends/Relaxation.h include/backends/branch-and-bound/RelaxationManager.h include/backends/branch-and-bound/Relaxations_Continuous.h include/modeling/annotations/Annotation.h src/modeling/annotations/impl_Annotation.cpp include/modeling/annotations/impl_Annotation.h include/backends/BranchAndBoundMIP.h include/backends/column-generation/Relaxations_DantzigWolfe.h src/backends/column-generation/Relaxations_DantzigWolfe.cpp include/backends/column-generation/ColumnGeneration.h src/backends/column-generation/ColumnGeneration.cpp include/backends/BranchAndPriceMIP.h include/modeling/models/BlockModel.h src/modeling/models/BlockModel.cpp include/modeling/models/AbstractModel.h)

#find_package(OpenMP REQUIRED)
#target_link_libraries(idol PUBLIC OpenMP::OpenMP_CXX)

if (${USE_ROOT})
    find_package(ROOT REQUIRED)
    target_link_libraries(idol PUBLIC ROOT::Core ROOT::Graf ROOT::Gpad)
    target_compile_definitions(idol PUBLIC IDOL_USE_ROOT)
endif()

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
        DESTINATION lib/cmake/Idol
        )
