add_executable(idol_cl
        idol_cl.cpp
        solve_milp.h
        solve_bilevel.h
        output.h
        MethodManager.cpp
        MethodManager.h
        VariableAnalysis.h
)
target_link_libraries(idol_cl PRIVATE idol)

include(FetchContent)

FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG origin/master
)

FetchContent_MakeAvailable(cxxopts)

target_link_libraries(idol_cl PRIVATE cxxopts::cxxopts)

install(TARGETS idol_cl
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
