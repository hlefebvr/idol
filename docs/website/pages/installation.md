# Installation guidelines

## Overview

Welcome to idol's installation guide! This document will help you set up idol on your system.

There are mainly to options to use idol. 

* Let CMake do the work for you and install idol automatically in your build directory.
* Manually install idol from source and link it to your project, e.g., if you are not using CMake.

The easiest is the first option, which is the recommended way to try idol.

[TOC]

## Automatic installation with CMake and FetchContent

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

## Manual installation from source (CMake + Make)

This section describes how to install idol from source using CMake and Make.
You may consider this as a more advanced option, but it is still very easy to do.

### Installation 

First, we need to download idol from GitHub.

```shell
git clone https://github.com/hlefebvr/idol.git
cd idol
```

Then, we follow the standard CMake build procedure.

```shell
mkdir build
cd build
cmake ..
make
```

If there are no errors, we can install idol using the following command.

```shell
sudo make install
```

That's it! idol is now installed on your system. The next section describes how to link idol to your project.

> Note that you probably want to use idol in combination with external optimization solvers like Gurobi or HiGHS.
> To do so, you need to install the solvers and set the corresponding CMake options.
> See the [CMake Configuration Options](#cmake-configuration-options) section for more details.
>
> A CMake option can be set using `-DMY_OPTION=VALUE`, e.g., `cmake -DUSE_GUROBI=ON ..`.

If you do not have sudo privileges, you can also install idol in a custom folder.
To do so, use the standard `CMAKE_INSTALL_PREFIX` option to change the destination folder as follows:

```shell
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
```

### Linking

Now that idol is installed, we can link it to our project.
To do so, we will assume that you have a basic `CMake` project with a `CMakeLists.txt` that looks like this:

```cmake
cmake_minimum_required(VERSION 3.23)
project(my_project)

add_executable(my_target main.cpp)
```

This file creates a target called `my_target` made of only one source file `main.cpp`.
Now, we show how to link idol to your target.

First, we need to find idol using the `find_package` command.

```cmake
find_package(idol REQUIRED)
```

Then, we can link idol to our target.

```cmake
target_link_libraries(my_target PUBLIC idol)
```

That's it! You can now use idol in your project.

## CMake Configuration Options

> Use `ccmake .` or `cmake-gui` for an interactive interface to set options.

### External Solver Options

idol typically requires external solvers to be used. 
The following options are available to enable or disable support for specific solvers.
You need to have the solvers installed on your system to use them and give hint to CMake where to find them.

#### Boolean Options

| CMake Option       | Description                            | Default | Example   |
|--------------------|----------------------------------------|---------|-----------|
| `USE_GUROBI`       | Enable Gurobi support                  | `OFF`   | `ON`      |
| `USE_CPLEX`        | Enable CPLEX support                   | `OFF`   | `ON`      |
| `USE_MOSEK`        | Enable MOSEK support                   | `OFF`   | `ON`      |
| `USE_GLPK`         | Enable GLPK support                    | `OFF`   | `ON`      |
| `USE_HIGHS`        | Enable HiGHS support                   | `OFF`   | `ON`      |
| `USE_OSI`          | Enable COIN-OR OSI interface           | `OFF`   | `ON`      |
| `USE_OSI_CLP`      | Use CLP through OSI                    | `OFF`   | `ON`      |
| `USE_OSI_CPLEX`    | Use CPLEX through OSI                  | `OFF`   | `ON`      |
| `USE_OSI_SYMPHONY` | Use Symphony through OSI               | `OFF`   | `ON`      |
| `USE_MIBS`         | Enable MIbS support (bilevel solver)   | `OFF`   | `ON`      |
| `USE_JULIA`        | Enable Julia support (JuMP, Coluna.jl) | `OFF`   | `ON`      |

#### Directory Paths

| CMake Option   | Description                                                                                         | Default Value                                         | Example                                              |
|----------------|-----------------------------------------------------------------------------------------------------|-------------------------------------------------------|------------------------------------------------------|
| `GUROBI_DIR`   | Path to Gurobi installation                                                                         | `$ENV{GUROBI_HOME}`                                   | `$HOME/gurobi1100/linux64`                           |
| `CPLEX_DIR`    | Path to CPLEX installation                                                                          | `$ENV{CPLEX_HOME}` or `/opt/ibm/ILOG/CPLEX_Studio...` | `/opt/ibm/ILOG/CPLEX_Studio2211`                     |
| `MOSEK_DIR`    | Path to MOSEK installation                                                                          | `$ENV{MOSEK_HOME}`                                    | `$HOME/mosek/10.1/tools/platform/linux64x86`         |
| `GLPK_DIR`     | Path to GLPK installation <br><small>also searches default system paths like `/usr/include`</small> | `$ENV{GLPK_HOME}`                                     | `$HOME/glpk`                                         |
| `COIN_OR_DIR`  | Path to COIN-OR installation                                                                        | `$ENV{COIN_OR_HOME}`                                  | `$HOME/coin-or/dist`                                 |
| `JULIA_DIR`    | Path to Julia installation                                                                          | `$ENV{JULIA_HOME}`                                    | `$HOME/.julia/juliaup/julia-1.11.5+0.x64.linux.gnu/` |


### Other Optional Dependencies

In this section, you can find the optional dependencies that idol can use to enhance its functionality.
These dependencies are not required for idol to work, but they can provide additional features or improve performance.
You can enable or disable them using the following CMake options.

#### Boolean Options

| CMake Option    | Description                                                                                   | Default Value  | Example  |
|-----------------|-----------------------------------------------------------------------------------------------|----------------|----------|
| `USE_EIGEN`     | Enable Eigen support (for converting convex quadratic expressions to Mosek's conic interface) | `OFF`          | `ON`     |
| `USE_ROOT`      | Enable ROOT support (for scientific data analysis library and plots, ...)                     | `OFF`          | `ON`     |
| `USE_TSL`       | Enable TSL support (for better ordered map and sparse map implementations than std)           | `OFF`          | `ON`     |
| `USE_ROBINHOOD` | Enable Robinhood support (for better ordered map and sparse map implementations than std)     | `OFF`          | `ON`     |

#### Directory Paths

| CMake Option           | Description                                                                                               | Default Value                 | Example           |
|------------------------|-----------------------------------------------------------------------------------------------------------|-------------------------------|-------------------|
| `EIGEN_DIR`            | Path to Eigen installation<br /><small>also searches in `/usr/include/` and `/usr/local/include/`</small> | `$ENV{EIGEN_HOME}`            | `$HOME/eigen`     |
| `TSL_OREDERED_MAP_DIR` | Path to Tessil/ordered-map installation                                                                   | `$ENV{TSL_OREDERED_MAP_HOME}` | `$HOME/tsl`       |
| `TSL_SPARSE_MAP_DIR`   | Path to Tessil/sparse-map installation                                                                    | `$ENV{TSL_SPARSE_MAP_HOME}`   | `$HOME/tsl`       |
| `ROBINHOOD_DIR`        | Path to martinus/robin-hood-hashing installation                                                          | `$ENV{ROBINHOOD_HOME}`        | `$HOME/robin_hood` |

### Development Options

| CMake Option     | Description                                 | Default Value | Example |
|------------------|---------------------------------------------|---------------|---------|
| `BUILD_EXAMPLES` | Create example targets                      | `OFF`         | `ON`    |
| `BUILD_TESTS`    | Create test targets                         | `OFF`         | `ON`    |
| `BUILD_DEV`      | Create dev target                           | `OFF`         | `ON`    |
| `TEST_COVERAGE`  | Enable test coverage options (requires gcc) | `OFF`         | `ON`    |
