\page cmake-options CMake configuration options

[TOC]

This page describes the CMake configuration options available for idol.
These options allow you to customize the build process and enable or disable specific features or dependencies.
You can set these options when running CMake to configure the build system.

> Use `ccmake .` or `cmake-gui` for an interactive interface to set options.

## External Solver Options

idol typically requires external solvers to be used.
The following options are available to enable or disable support for specific solvers.
You need to have the solvers installed on your system to use them and give hint to CMake where to find them.

### Boolean Options

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

### Directory Paths

| CMake Option   | Description                                                                                         | Default Value                                         | Example                                              |
|----------------|-----------------------------------------------------------------------------------------------------|-------------------------------------------------------|------------------------------------------------------|
| `GUROBI_DIR`   | Path to Gurobi installation                                                                         | `$ENV{GUROBI_HOME}`                                   | `$HOME/gurobi1100/linux64`                           |
| `CPLEX_DIR`    | Path to CPLEX installation                                                                          | `$ENV{CPLEX_HOME}` or `/opt/ibm/ILOG/CPLEX_Studio...` | `/opt/ibm/ILOG/CPLEX_Studio2211`                     |
| `MOSEK_DIR`    | Path to MOSEK installation                                                                          | `$ENV{MOSEK_HOME}`                                    | `$HOME/mosek/10.1/tools/platform/linux64x86`         |
| `GLPK_DIR`     | Path to GLPK installation <br><small>also searches default system paths like `/usr/include`</small> | `$ENV{GLPK_HOME}`                                     | `$HOME/glpk`                                         |
| `COIN_OR_DIR`  | Path to COIN-OR installation                                                                        | `$ENV{COIN_OR_HOME}`                                  | `$HOME/coin-or/dist`                                 |
| `JULIA_DIR`    | Path to Julia installation                                                                          | `$ENV{JULIA_HOME}`                                    | `$HOME/.julia/juliaup/julia-1.11.5+0.x64.linux.gnu/` |


## Other Optional Dependencies

In this section, you can find the optional dependencies that idol can use to enhance its functionality.
These dependencies are not required for idol to work, but they can provide additional features or improve performance.
You can enable or disable them using the following CMake options.

### Boolean Options

| CMake Option    | Description                                                                                   | Default Value  | Example  |
|-----------------|-----------------------------------------------------------------------------------------------|----------------|----------|
| `USE_EIGEN`     | Enable Eigen support (for converting convex quadratic expressions to Mosek's conic interface) | `OFF`          | `ON`     |
| `USE_ROOT`      | Enable ROOT support (for scientific data analysis library and plots, ...)                     | `OFF`          | `ON`     |
| `USE_TSL`       | Enable TSL support (for better ordered map and sparse map implementations than std)           | `OFF`          | `ON`     |
| `USE_ROBINHOOD` | Enable Robinhood support (for better ordered map and sparse map implementations than std)     | `OFF`          | `ON`     |

### Directory Paths

| CMake Option           | Description                                                                                               | Default Value                 | Example           |
|------------------------|-----------------------------------------------------------------------------------------------------------|-------------------------------|-------------------|
| `EIGEN_DIR`            | Path to Eigen installation<br /><small>also searches in `/usr/include/` and `/usr/local/include/`</small> | `$ENV{EIGEN_HOME}`            | `$HOME/eigen`     |
| `TSL_OREDERED_MAP_DIR` | Path to Tessil/ordered-map installation                                                                   | `$ENV{TSL_OREDERED_MAP_HOME}` | `$HOME/tsl`       |
| `TSL_SPARSE_MAP_DIR`   | Path to Tessil/sparse-map installation                                                                    | `$ENV{TSL_SPARSE_MAP_HOME}`   | `$HOME/tsl`       |
| `ROBINHOOD_DIR`        | Path to martinus/robin-hood-hashing installation                                                          | `$ENV{ROBINHOOD_HOME}`        | `$HOME/robin_hood` |

## Development Options

| CMake Option     | Description                                 | Default Value | Example |
|------------------|---------------------------------------------|---------------|---------|
| `BUILD_EXAMPLES` | Create example targets                      | `OFF`         | `ON`    |
| `BUILD_TESTS`    | Create test targets                         | `OFF`         | `ON`    |
| `BUILD_DEV`      | Create dev target                           | `OFF`         | `ON`    |
| `TEST_COVERAGE`  | Enable test coverage options (requires gcc) | `OFF`         | `ON`    |
