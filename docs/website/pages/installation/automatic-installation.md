\page automatic-installation Automatic installation with CMake

Let's assume that you have a basic `CMake` project with a `CMakeLists.txt` that looks like this:

```cmake 
cmake_minimum_required(VERSION 3.23)
project(my_project)

add_executable(my_target main.cpp)
```

This file creates a target called `my_target` made of only one source file `main.cpp`.
Now, we show how to download idol and link it to your target.

First, we use `FetchContent` to download idol.

```cmake
include(FetchContent)

FetchContent_Declare(
        idol
        GIT_REPOSITORY https://github.com/hlefebvr/idol.git
        GIT_TAG        origin/main # Here, you can put a specific tag (e.g., v1.0.0) or a commit hash
)

# Ask CMake to download idol and install it to your build directory
FetchContent_MakeAvailable(idol)
```

Then, we can link idol to our target.

```cmake
target_link_libraries(my_target PUBLIC idol)
```

That's it! You can now use idol in your project.

> Note that you probably want to use idol in combination with external optimization solvers like Gurobi or HiGHS.
> To do so, you need to install the solvers and set the corresponding CMake options.
> See the [CMake Configuration Options](#cmake-configuration-options) section for more details.
>
> A CMake option can be set using `set(MY_OPTION VALUE)` before the `FetchContent_MakeAvailable(idol)` line.
