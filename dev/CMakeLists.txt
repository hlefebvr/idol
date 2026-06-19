add_executable(dev
        main.cpp
)

find_package(Boost REQUIRED)

target_link_libraries(dev PRIVATE
        idol
        Boost::boost
)