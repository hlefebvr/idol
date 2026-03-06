add_executable(idol_cl
        idol_cl.cpp
        solve_milp.h
        solve_bilevel.h
        solve_robust.h
        output.h
        MethodManager.cpp
        MethodManager.h
        VariableAnalysis.h
        Arguments.h
        Arguments.cpp
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
