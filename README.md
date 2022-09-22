# optimize

## Compiling instructions

### Building the library and other targets

The library compilation is managed by `CMake`.

To compile the library only, *without any external solver*, you may run the following.
```shell
mkdir build
cd build/
cmake ..
make optimize
```

To link with external solvers, or for other purposes, use the following options (`cmake .. -D<OPTION_NAME>=<VALUE>`). 

| Parameter        | Effect                                                                                                                                                                                     |
|------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `USE_GUROBI`     | When set to `YES`, links `optimize` with the gurobi solver and adds related classes.                                                                                                       |
| `GUROBI_DIR`     | May be used to specify the installation directory of the gurobi solver. If not, the environment variable `GUROBI_HOME` is used.                                                            |
| `USE_LPSOLVE`    | When set to `YES`, links `optimize` with the lpsolve solver and adds the related classes.                                                                                                  |
| `LPSOLVE_DIR`    | May be used to specify the installation directory of the lpsolve solver. If not the library is looked for in `/usr/include/lpsolve` and `/usr/bin/lpsolve` (default installation folders). |
| `BUILD_EXAMPLES` | When set to `YES`, creates the example targets so as to build examples.                                                                                                                    |
| `BUILD_TESTS`    | When set to `YES`, creates the `tests` target so as to run unitary tests.                                                                                                                  |
| `TEST_COVERAGE`  | WHen set to `YES` and `BUILD_TESTS=YES`, prepares the `tests` target for test coverage evaluation (links with `gcov`, use `gcc` as compiler, etc.)                                         |

### Unitary tests

Unitary tests depend on the `Catch2` library. 
If `BUILD_TESTS=YES` and if `Catch2` cannot be found, it will automatically be downloaded from the [official Catch2 repository](https://github.com/catchorg/Catch2).

### Test coverage

Evaluating code coverage is done using `lcov` which requires `gcc` to be used a compiler. When the `TEST_COVERAGE=YES` option is used along with `BUILD_TESTS=YES`, be sure to have these tools installed.
To build the code coverage evaluation report, simply run the following.
```shell
./build_coverage.sh
```

### Documentation

The documentation is built using `doxygen`. To build it, run the following command.
```shell
./build_documentation.sh
```