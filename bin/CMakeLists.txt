add_executable(idol_cli
    idol_cli.cpp
)
target_link_libraries(idol_cli PRIVATE idol)

install(TARGETS idol_cli
        RUNTIME DESTINATION bin
)

include(FetchContent)

FetchContent_Declare(
        cxxopts
        GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
        GIT_TAG origin/master
)

FetchContent_MakeAvailable(cxxopts)

target_link_libraries(idol_cli PRIVATE cxxopts::cxxopts)