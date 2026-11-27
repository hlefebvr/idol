\page solver_configuration External Solver Configuration
\brief Configure external solvers for use with idol and idol_cl.

\tableofcontents

For a summary of which problem classes and capabilities each solver supports through idol, see \ref solver_compatibility.

idol supports external solvers in two ways:

- **Runtime loading** — the solver library is located and loaded when you first call it, without recompiling idol. Configuration is done via environment variables. This applies to Gurobi, HiGHS, GLPK, CPLEX and Julia/JuMP.
- **Build-time linking** — solver support is compiled into idol. You enable it with a CMake option and must rebuild. This applies to MibS, Cgl and Osi.

Package manager installations (`apt-get`, `brew`) include Gurobi, HiGHS, GLPK, CPLEX, MibS and Cgl automatically. For source builds, see \ref build_from_source.

---

\section solver_config_runtime Runtime-Configured Solvers

These solvers are located and loaded at runtime via `dlopen`. No rebuild is required — set the environment variable and restart.

For each solver, idol checks an `IDOL_*_PATH` variable first. If that is not set, it falls back to a solver-specific home variable or a list of common system paths. All variables are **case-sensitive**.

\subsection solver_config_gurobi Gurobi

\note Requires a valid Gurobi license. See <a href="https://www.gurobi.com/documentation/" target="_blank">gurobi.com</a>.

| Variable | Type | Description |
|:---------|:-----|:------------|
| `IDOL_GUROBI_PATH` | File path | Explicit path to `libgurobi*.dylib` / `libgurobi*.so`. Takes priority over all other variables. |
| `GUROBI_HOME` | Directory | Root of the Gurobi installation. idol looks for the library in `$GUROBI_HOME/lib/`. Normally set automatically by the Gurobi installer. |

There are no other fallback paths. If neither variable is set and Gurobi is not in a standard system location, idol will not find it.

**Examples:**
```shell
# Option 1 — explicit library file (highest priority)
export IDOL_GUROBI_PATH=/Library/gurobi1100/macos_universal2/lib/libgurobi110.dylib

# Option 2 — installation root (set by Gurobi installer)
export GUROBI_HOME=/Library/gurobi1100/macos_universal2
```

\subsection solver_config_highs HiGHS

HiGHS is free and open-source (MIT license). See <a href="https://highs.dev" target="_blank">highs.dev</a>.

| Variable | Type | Description |
|:---------|:-----|:------------|
| `IDOL_HiGHS_PATH` | File path | Explicit path to `libhighs.dylib` / `libhighs.so`. Takes priority over all fallbacks. |

If `IDOL_HiGHS_PATH` is not set, idol searches these locations in order:

```text
/opt/homebrew/opt/highs/lib/libhighs.dylib   (macOS, Homebrew)
/usr/local/opt/highs/lib/libhighs.dylib
/opt/homebrew/lib/libhighs.dylib
/usr/local/lib/libhighs.dylib
/usr/lib/libhighs.so                          (Linux)
/usr/lib64/libhighs.so
/usr/local/lib/libhighs.so
/usr/local/lib64/libhighs.so
/lib/x86_64-linux-gnu/libhighs.so
/usr/lib/x86_64-linux-gnu/libhighs.so
libhighs.dylib                                (system library path)
libhighs.so
```

**Example:**
```shell
export IDOL_HiGHS_PATH=/opt/homebrew/opt/highs/lib/libhighs.dylib
```

\subsection solver_config_glpk GLPK

GLPK is free and open-source (GPL v3). See <a href="https://www.gnu.org/software/glpk/" target="_blank">gnu.org/software/glpk</a>.

| Variable | Type | Description |
|:---------|:-----|:------------|
| `IDOL_GLPK_PATH` | File path | Explicit path to `libglpk.dylib` / `libglpk.so`. Takes priority over all fallbacks. |

If `IDOL_GLPK_PATH` is not set, idol searches these locations in order:

```text
/opt/homebrew/opt/glpk/lib/libglpk.dylib     (macOS, Homebrew)
/usr/local/opt/glpk/lib/libglpk.dylib
/opt/homebrew/lib/libglpk.dylib
/usr/local/lib/libglpk.dylib
/usr/lib/libglpk.so                           (Linux)
/usr/lib64/libglpk.so
/usr/local/lib/libglpk.so
/usr/local/lib64/libglpk.so
/lib/x86_64-linux-gnu/libglpk.so
/usr/lib/x86_64-linux-gnu/libglpk.so
libglpk.dylib                                 (system library path)
libglpk.so
```

**Example:**
```shell
export IDOL_GLPK_PATH=/opt/homebrew/lib/libglpk.dylib
```

\subsection solver_config_cplex CPLEX

\note Requires a valid CPLEX license. See <a href="https://www.ibm.com/products/ilog-cplex-optimization-studio" target="_blank">ibm.com</a>.

CPLEX is configured via environment variables at runtime.

| Variable | Type | Description |
|:---------|:-----|:------------|
| `IDOL_CPLEX_PATH` | File path | Explicit path to `libcplex*.dylib` / `libcplex*.so` / `cplex*.dll`. Takes priority. |
| `CPLEX_STUDIO_DIR2211` | Directory | Root of a CPLEX Studio 22.1.1 installation. |
| `CPLEX_STUDIO_DIR` | Directory | Root of any CPLEX Studio installation. |
| `CPLEX_HOME` | Directory | Alternative CPLEX root directory. |

When a home directory is found, idol recursively searches `{home}/cplex/bin/` and `{home}/bin/` for a file matching `libcplex[0-9]+.dylib` (macOS), `libcplex[0-9]+.so` (Linux) or `cplex[0-9]+.dll` (Windows).

**Examples:**
```shell
# Option 1 — explicit library file
export IDOL_CPLEX_PATH=/opt/ibm/ILOG/CPLEX_Studio2211/cplex/bin/x86-64_linux/libcplex2211.so

# Option 2 — studio root directory
export CPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio2211
```

\subsection solver_config_julia JuMP / Julia

JuMP is free and open-source (MIT license). See <a href="https://jump.dev" target="_blank">jump.dev</a>.

| Variable | Type | Description |
|:---------|:-----|:------------|
| `IDOL_JULIA_PATH` | File path | Explicit path to `libjulia.dylib` / `libjulia.so`. Takes priority over all fallbacks. |

If `IDOL_JULIA_PATH` is not set, idol searches these locations in order:

```text
/opt/homebrew/opt/julia/lib/libjulia.dylib   (macOS, Homebrew)
/usr/local/opt/julia/lib/libjulia.dylib
/opt/homebrew/lib/libjulia.dylib
/usr/local/lib/libjulia.dylib
/usr/lib/libjulia.so                          (Linux)
/usr/lib64/libjulia.so
/usr/local/lib/libjulia.so
/usr/local/lib64/libjulia.so
/lib/x86_64-linux-gnu/libjulia.so
/usr/lib/x86_64-linux-gnu/libjulia.so
libjulia.so                                   (system library path)
libjulia.dylib
```

---

\section solver_config_buildtime Build-Time-Configured Solvers

These solvers are compiled into idol at build time. You must pass the appropriate CMake option and rebuild. All of them use a COIN-OR installation pointed to by `COIN_OR_DIR` (CMake variable) or `COIN_OR_HOME` (environment variable); both must point to the COIN-OR `dist/` directory.

\subsection solver_config_mibs MibS

MibS is the mixed-integer bilevel solver, published under the Eclipse Public License v2.0.

\note idol requires the customized fork <a href="https://github.com/hlefebvr/MibS" target="_blank">hlefebvr/MibS</a> (branch `idol`), not the upstream coin-or/MibS release. The fork enables using any idol-compatible solver (e.g., Gurobi) as a feasibility checker inside MibS.

| CMake option | Default | Description |
|:-------------|:--------|:------------|
| `USE_MIBS=ON` | OFF | Enable MibS support. Automatically enables `USE_CGL`, `USE_OSI_CLP` and `USE_OSI`. |
| `COIN_OR_DIR` | — | Path to the COIN-OR `dist/` directory. Falls back to `$COIN_OR_HOME`. |

```shell
cmake -DUSE_MIBS=ON -DCOIN_OR_DIR=/path/to/coin-or/dist ..
```

For build instructions, see \ref cmake_mibs.

\subsection solver_config_cgl Cgl

Cgl is the COIN-OR cut generation library, published under the Eclipse Public License.

| CMake option | Default | Description |
|:-------------|:--------|:------------|
| `USE_CGL=ON` | OFF | Enable Cgl support. Automatically enables `USE_OSI`. |
| `COIN_OR_DIR` | — | Path to the COIN-OR `dist/` directory. Falls back to `$COIN_OR_HOME`. |

```shell
cmake -DUSE_CGL=ON -DCOIN_OR_DIR=/path/to/coin-or/dist ..
```

\subsection solver_config_osi Osi

Osi is the COIN-OR solver interface, giving access to any Osi-compatible solver.

| CMake option | Default | Description |
|:-------------|:--------|:------------|
| `USE_OSI=ON` | OFF | Enable the base Osi wrapper. |
| `USE_OSI_CLP=ON` | OFF | Enable the CLP sub-solver. |
| `USE_OSI_SYMPHONY=ON` | OFF | Enable SYMPHONY. |
| `COIN_OR_DIR` | — | Path to the COIN-OR `dist/` directory. |

---

\section solver_config_troubleshooting Troubleshooting

If idol reports that a solver cannot be found after setting the variables above, see \ref cannot_find_solver.
