# idol, A C++ Framework for Optimization 

![License](https://img.shields.io/github/license/hlefebvr/idol?color=blue)
![GitHub tag (latest by date)](https://img.shields.io/github/v/release/hlefebvr/idol?color=blue)
![GitHub Workflow Status (branch)](https://github.com/hlefebvr/idol/actions/workflows/tests.yml/badge.svg)
![GitHub issues](https://img.shields.io/github/issues-raw/hlefebvr/idol)
![Repo status](https://www.repostatus.org/badges/latest/active.svg)
[![codecov](https://codecov.io/github/hlefebvr/idol/branch/main/graph/badge.svg?token=BWMH5522QP)](https://app.codecov.io/gh/hlefebvr/idol)
[![GitHub sponsors](https://img.shields.io/github/sponsors/hlefebvr)](https://github.com/sponsors/hlefebvr)

## What is idol?

idol is a C++ framework for mathematical optimization and complex decision-making problems. It is designed to help you build new algorithms for solving complex optimization problems. The main philosophy behind idol is interoperability and ease of use. Hence, any algorithm can be seamlessly combined with any other algorithm to create a new one. 

For instance, you can combine a branch-and-bound algorithm with a column generation algorithm to create a branch-and-price algorithm.

```cpp
const auto branch_and_price = branch_and_bound + column_generation;
model.use(branch_and_price);
model.optimize();
```

With idol, you can also interface with your favorite solver:

- Mixed-integer optimization ([Gurobi](https://www.gurobi.com/), [Cplex](https://www.ibm.com/products/ilog-cplex-optimization-studio), [HiGHS](https://github.com/ERGO-Code/HiGHS), [GLPK](https://www.gnu.org/software/glpk/), [coin-or/Osi](https://github.com/coin-or/Osi/) and the Julia framework [JuMP](https://github.com/jump-dev/JuMP.jl));
- Bilevel optimization ([coin-or/MibS](https://github.com/coin-or/MibS));

## Documentation

Visit our [online documentation](https://hlefebvr.github.io/idol/).

## Installation

### On Linux (amd64)

```shell
echo "deb [arch=amd64 trusted=yes] https://henrilefebvre.com/apt stable main" \
| sudo tee /etc/apt/sources.list.d/idol.list
sudo apt update
sudo apt install idol
```

### On Mac (arm64)

```shell
brew tap hlefebvr/idol 
brew update
brew install idol
```

### From Source (Expert Users)

```
git clone https://github.com/hlefebvr/idol.git
cd idol
mkdir build
cd build
cmake ..
make
sudo make install
```

## Using idol for Research?

If you're interested in idol or use it for research, do not hesitate to contact me at henri.lefebvre@cnrs.fr.
