add_executable(idol_cli
    idol_cli.cpp
)
target_link_libraries(idol_cli PRIVATE idol)

install(TARGETS idol_cli
        RUNTIME DESTINATION bin
)
