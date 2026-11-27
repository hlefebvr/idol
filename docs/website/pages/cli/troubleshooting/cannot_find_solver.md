\page cannot_find_solver Cannot find solver
\brief What to do when idol cannot locate an installed solver.

If idol reports that a solver cannot be found, the solver library is either not
in a standard location or idol does not know where to look.

**Step 1 — set the direct path variable.**
Every runtime-loaded solver has an `IDOL_*_PATH` variable that takes priority
over all automatic search:

| Solver | Variable | Example value |
|:-------|:---------|:--------------|
| Gurobi | `IDOL_GUROBI_PATH` | `/Library/gurobi1100/macos_universal2/lib/libgurobi110.dylib` |
| HiGHS | `IDOL_HiGHS_PATH` | `/opt/homebrew/opt/highs/lib/libhighs.dylib` |
| GLPK | `IDOL_GLPK_PATH` | `/opt/homebrew/lib/libglpk.dylib` |
| CPLEX | `IDOL_CPLEX_PATH` | `/opt/ibm/ILOG/CPLEX_Studio2211/cplex/bin/x86-64_linux/libcplex2211.so` |
| Julia/JuMP | `IDOL_JULIA_PATH` | `/opt/homebrew/opt/julia/lib/libjulia.dylib` |

Set the variable and retry. For the full list of fallback paths and all
configuration options, see \ref solver_configuration.

**Step 2 — MibS not found.**
MibS is compiled in at build time, not loaded at runtime. If idol was installed
via a package manager, try reinstalling. If you built from source, check that
`USE_MIBS=ON` and that `COIN_OR_DIR` still points to the original `dist/`
directory — moving it after the build breaks the paths. See \ref cmake_mibs.
