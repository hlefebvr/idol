add_executable(idol_cl
        idol_cl.cpp
        milp.h
        solve_bilevel.h
        robust.h
        output.h
        method-managers/MethodManager.cpp
        method-managers/MethodManager.h
        VariableAnalysis.h
        Arguments.h
        Arguments.cpp
        method-managers/robust/RobustMethodManager.cpp
        method-managers/robust/RobustMethodManager.h
        method-managers/AbstractMethodManager.h
        method-managers/robust/CCG_Farkas.cpp
        method-managers/robust/CCG_Farkas.h
        method-managers/robust/CCG_KKT_SOS1.cpp
        method-managers/robust/CCG_KKT_SOS1.h
        method-managers/robust/CCG_MibS.cpp
        method-managers/robust/CCG_MibS.h
        method-managers/robust/CCG_CV.cpp
        method-managers/robust/CCG_CV.h
        method-managers/robust/CG_Indicator.cpp
        method-managers/robust/CG_Indicator.h
        method-managers/milp/MILPMethodManager.cpp
        method-managers/milp/MILPMethodManager.h
        method-managers/milp/Gurobi.cpp
        method-managers/milp/Gurobi.h
        method-managers/milp/GLPK.cpp
        method-managers/milp/GLPK.h
        method-managers/milp/HiGHS.cpp
        method-managers/milp/HiGHS.h
        method-managers/milp/Cplex.cpp
        method-managers/milp/Cplex.h
        method-managers/milp/JuMP.cpp
        method-managers/milp/JuMP.h
)
target_link_libraries(idol_cl PRIVATE idol)

include(FetchContent)

FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.6.2
)

FetchContent_MakeAvailable(CLI11)
target_link_libraries(idol_cl PRIVATE CLI11::CLI11)

install(TARGETS idol_cl
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
