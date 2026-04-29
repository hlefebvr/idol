find_path(YASOL_SOLVER_INCLUDE_DIR
        NAMES Algorithms
        PATHS
        ${YASOL_DIR}/Solver/include
        /usr/local/include
        /usr/include
)

find_path(YASOL_QBPSOLVER_INCLUDE_DIR
        NAMES QBPSolver.h
        PATHS
        ${YASOL_DIR}/QBPSolver/src
        /usr/local/include
        /usr/include
)

find_path(YASOL_YASOL_INCLUDE_DIR
        NAMES yInterface.h
        PATHS
        ${YASOL_DIR}/Yasol/src
        /usr/local/include
        /usr/include
)

find_library(YASOL_SOLVER_LIBRARY
        NAMES SolverLib
        PATHS
        ${YASOL_DIR}/build/lib
        /usr/local/lib
        /usr/lib
)

find_library(YASOL_QBPSOLVER_LIBRARY
        NAMES QBPSolverLib
        PATHS
        ${YASOL_DIR}/build/lib
        /usr/local/lib
        /usr/lib
)

find_library(YASOL_YASOL_LIBRARY
        NAMES YasolLib
        PATHS
        ${YASOL_DIR}/build/lib
        /usr/local/lib
        /usr/lib
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Yasol
        REQUIRED_VARS
        # Libraries
        YASOL_SOLVER_LIBRARY
        YASOL_QBPSOLVER_LIBRARY
        YASOL_YASOL_LIBRARY
        # Include directories
        YASOL_SOLVER_INCLUDE_DIR
        YASOL_QBPSOLVER_INCLUDE_DIR
        YASOL_YASOL_INCLUDE_DIR
)

if(YASOL_FOUND)
    add_library(Yasol::Yasol UNKNOWN IMPORTED)
    set_target_properties(Yasol::Yasol PROPERTIES
            IMPORTED_LOCATION  "${YASOL_YASOL_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${YASOL_SOLVER_INCLUDE_DIR};${YASOL_YASOL_INCLUDE_DIR};${YASOL_QBPSOLVER_INCLUDE_DIR}"
    )
    target_link_libraries(Yasol::Yasol INTERFACE ${YASOL_QBPSOLVER_LIBRARY} ${YASOL_SOLVER_LIBRARY})
endif()