# idol — A C++ Framework for Mathematical Optimization

![License](https://img.shields.io/github/license/hlefebvr/idol?color=blue)
![GitHub tag (latest by date)](https://img.shields.io/github/v/release/hlefebvr/idol?color=blue)
![GitHub Workflow Status (branch)](https://github.com/hlefebvr/idol/actions/workflows/tests.yml/badge.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/active.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)

**Website:** https://henrilefebvre.com/idol/ &nbsp;|&nbsp;
**Documentation:** https://hlefebvr.github.io/idol/ &nbsp;|&nbsp;
**Contact:** henri.lefebvre@cnrs.fr

---

idol is a C++ framework for tackling complex mathematical optimization problems.
It is built around a single design principle: **algorithms should compose**.
Combine a branch-and-bound with a column generation, and you get a branch-and-price — in one line.

```cpp
const auto branch_and_price = branch_and_bound + column_generation;
model.use(branch_and_price);
model.optimize();
```

idol is **not** a solver itself. It works hand in hand with existing solvers (Gurobi, HiGHS, CPLEX, …) and lets you build, combine, and experiment with advanced algorithms on top of them.

The framework ships in two forms:

- **`idol` (C++ library)** — for building custom algorithms programmatically.
- **`idol_cl` (command-line interface)** — for solving MILP, bilevel, and robust problems directly from the terminal, without writing C++ code.

---

## Table of Contents

- [Installation](#installation)
- [idol\_cl — Command-Line Interface](#idol_cl--command-line-interface)
- [idol — C++ Library](#idol--c-library)
- [Supported Solvers](#supported-solvers)
- [Using idol for Research?](#using-idol-for-research)

---

## Installation

### Linux (amd64) — via apt

```shell
echo "deb [arch=amd64 trusted=yes] https://henrilefebvre.com/apt stable main" \
  | sudo tee /etc/apt/sources.list.d/idol.list
sudo apt-get update && sudo apt-get install idol
```

### macOS (arm64) — via Homebrew

```shell
brew tap hlefebvr/idol && brew update && brew install idol
```

Verify your installation with `idol_cl --version`.

### From Source — using the install script

The recommended source build. Automates fetching and building MibS (COIN-OR bilevel solver) alongside idol.

```shell
wget https://raw.githubusercontent.com/hlefebvr/idol/main/install_idol.sh
chmod +x install_idol.sh
./install_idol.sh
```

idol is installed in `./idol/dist` after completion.

### From Source — using CMake

For full control over build options and install prefix:

```shell
git clone https://github.com/hlefebvr/idol.git
cd idol && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) && sudo make install
```

Key CMake options: `-DUSE_MIBS=YES`, `-DBUILD_EXAMPLES=YES`, `-DCOIN_OR_DIR=<path>`.

---

## idol\_cl — Command-Line Interface

`idol_cl` solves MILP, bilevel, and robust optimization problems from the shell.
It reads standard `.lp` and `.mps` model files, with auxiliary files for bilevel and robust descriptions.

```
idol_cl <problem> <sub-command> [file] [options]
```

**Problem types:** `milp` | `bilevel` | `robust`  
**Sub-commands:** `solve` | `list`

Use `list` to print all available solution methods for a given problem type.

### Common Options

| Flag | Description |
|---|---|
| `--method <name>` | Solution method |
| `--time-limit <s>` | Wall-clock time limit in seconds |
| `--thread-limit <n>` | Maximum number of threads |
| `--csv-report` | Also print a machine-readable CSV summary |
| `--config, -c <file.ini>` | Load options from a configuration file |
| `--version, -v` | Print version and detected solver availability |

### Solving a MILP

```shell
idol_cl milp list                                              # list available methods
idol_cl milp solve problem.lp --method HiGHS
idol_cl milp solve problem.mps --method Gurobi --time-limit 300 --thread-limit 4
```

Available methods: `Gurobi`, `HiGHS`, `Cplex`, `GLPK`, `JuMP`.

### Solving a Bilevel Problem

Bilevel problems are described by a `.lp`/`.mps` file (high-point relaxation) and an `.aux` file annotating the lower-level variables and constraints.

```shell
idol_cl bilevel list
idol_cl bilevel list problem.lp --aux problem.aux      # list methods that are applicable to your problem
idol_cl bilevel solve problem.lp --aux problem.aux --method KKT_SOS1
idol_cl bilevel solve problem.lp --aux problem.aux --method MibS --time-limit 600
idol_cl bilevel solve problem.lp --aux problem.aux --method KKT_SOS1 --pessimistic
```

Available methods: `KKT_SOS1`, `KKT_BigM`, `KKT_NLP`, `MibS`, `StrongDuality`, and many others.

See the [documentation](https://hlefebvr.github.io/idol/) for details on the `.aux` file format and method-specific options (`--pessimistic`, `--no-kleinert-vi`, `--bound-provider`, …).

### Solving a Robust Problem

Robust problems require a `.lp`/`.mps` deterministic model, a `.par` file identifying uncertain variables, and a `.unc` file defining the uncertainty set. Two-stage problems additionally need an `.aux` file marking the second-stage variables.

```shell
idol_cl robust list
idol_cl robust list problem.lp --par uncertainty.par --unc uncertainty_set.lp # lists methods that are applicable to your problem 
idol_cl robust solve problem.lp --par uncertainty.par --unc uncertainty_set.lp \
    --aux stages.aux --method CCG_Farkas         # two-stage (adjustable)
```

Available methods: `CCG_Farkas`, `CCG_KKT_SOS1`, `CCG_MibS`, `CCG_CV`, `CG_Indicator`, `Convexification`, and many others.

---

## idol — C++ Library

The C++ library lets you build and compose optimization algorithms programmatically (C++20).
See the [online documentation](https://hlefebvr.github.io/idol/) for tutorials and a full API reference.

### Composable algorithms

The central idea is that algorithms are objects that combine naturally:

```cpp
const auto branch_and_bound = BranchAndBound()
        .with_branching_rule(MostInfeasible())
        .with_node_selection_rule(BestBound());
        
const auto column_generation = DantzigWolfeDecomposition(decomposition)
        .with_master_optimizer(Gurobi::ContinuousRelaxation())
        .with_default_sub_problem_spec(DantzigWolfe::SubProblem().add_optimizer(Gurobi()));

const auto branch_and_price =  branch_and_bound + column_generation;

model.use(branch_and_price);
model.optimize();
```

The same pattern applies to bilevel and robust algorithms:

```cpp
// Bilevel: KKT single-level reformulation solved by Gurobi

model.use(Bilevel::KKT() + Gurobi());

// Robust: column-and-constraint generation with a Gurobi adversary

auto ccg = idol::Robust::ColumnAndConstraintGeneration(robust_description, bilevel_description);
ccg.with_initial_scenario_by_maximization(*sub_milp_optimizer);
ccg.with_master_optimizer(*sub_milp_optimizer);

auto kkt = idol::Bilevel::KKT();
kkt.with_sos1_constraints(true);
kkt.with_single_level_optimizer(*sub_milp_optimizer);

auto optimality_separation = idol::Robust::CCG::OptimalitySeparation();
optimality_separation.with_bilevel_optimizer(kkt);

ccg.add_separation(optimality_separation);

model.use(ccg);
```

### Interfacing with solvers

Switch the underlying solver without rewriting your model:

```cpp
model.use(Gurobi());   // or HiGHS(), GLPK(), JuMP(), and others!
model.optimize();
```

---

## Supported Solvers

| Solver | License | Notes                                                    |
|---|---|----------------------------------------------------------|
| [Gurobi](https://www.gurobi.com/) | Commercial | Free academic license available                          |
| [CPLEX](https://www.ibm.com/products/ilog-cplex-optimization-studio) | Commercial | Free academic license available                          |
| [HiGHS](https://github.com/ERGO-Code/HiGHS) | Open source | Free for all                                             |
| [GLPK](https://www.gnu.org/software/glpk/) | Open source | Free for all                                             |
| [coin-or/Osi](https://github.com/coin-or/Osi) | Open source |                                                          |
| [JuMP](https://github.com/jump-dev/JuMP.jl) (Julia) | Open source | Via embedded Julia bridge (can use any solver from JuMP) |
| [coin-or/MibS](https://github.com/coin-or/MibS) | Open source |                                                          |

Run `idol_cl --version` to see which solvers are available in your installation.

---

## Using idol for Research?

If you use idol in a research project or publication, feel free to reach out:
**henri.lefebvre@cnrs.fr**

Sponsoring the project is also very welcome:
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)
