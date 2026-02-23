\page cannot_find_solver Cannot find Cplex, GLPK, Gurobi, HiGHS, or any other solver
\brief Describes what to do if an external solver is installed but not detected by `idol_cl`.

\tableofcontents

If `idol` reports that a solver cannot be found, it usually means the solver library is not located in a standard system directory, or its location is not known to `idol`.


Solver libraries are searched in directories in the following order:

1. A solver-specific environment variable (recommended)
2. Standard system installation directories
3. Default library names available in the system library path

<div class="tabbed">
<ul>
<li>
<b class="tab-title">GLPK</b>
To help `idol` locate GLPK, you can set the environment variable `IDOL_GLPK_PATH`. 

This variable should point to the GLPK library file.

**Example**:
```shell
export IDOL_GLPK_PATH=/path/to/libglpk.so
```

If this variable is not set, `idol` will automatically search common system locations, including:

- `/opt/homebrew/opt/glpk/lib/libglpk.dylib`
- `/usr/local/opt/glpk/lib/libglpk.dylib`
- `/opt/homebrew/lib/libglpk.dylib`
- `/usr/local/lib/libglpk.dylib`
- `/usr/lib/libglpk.so`
- `/usr/lib64/libglpk.so`
- `/usr/local/lib/libglpk.so`
- `/usr/local/lib64/libglpk.so`
- `/lib/x86_64-linux-gnu/libglpk.so`
- `/usr/lib/x86_64-linux-gnu/libglpk.so`
- `libglpk.dylib`
- `libglpk.so`

</li>
<li>
<b class="tab-title">Gurobi</b>
To help `idol` locate Gurobi, you can set the environment variable `IDOL_GUROBI_PATH`. 

This variable should point to the Gurobi library file.

**Example**:
```shell
IDOL_GUROBI_PATH=/Library/gurobi1300/macos_universal2/lib/libgurobi130.dylib
```

If this variable is not set, `idol` will automatically search the folder indicated by `GUROBI_HOME` variable.

This variable is normally configured automatically when installing Gurobi.

**Example**:
```shell
GUROBI_HOME=/Library/gurobi1300/macos_universal2
```
</li>
<li>
<b class="tab-title">HiGHS</b>
To help `idol` locate HiGHS, you can set the environment variable `IDOL_HiGHS_PATH`.

This variable should point to the HiGHS library file.

**Example**:
```shell
export IDOL_HiGHS_PATH=/path/to/libhighs.so
```

If this variable is not set, `idol` will automatically search common system locations, including:
- `/opt/homebrew/opt/highs/lib/libhighs.dylib`
- `/usr/local/opt/highs/lib/libhighs.dylib`
- `/opt/homebrew/lib/libhighs.dylib`
- `/usr/local/lib/libhighs.dylib`
- `/usr/lib/libhighs.so`
- `/usr/lib64/libhighs.so`
- `/usr/local/lib/libhighs.so`
- `/usr/local/lib64/libhighs.so`
- `/lib/x86_64-linux-gnu/libhighs.so`
- `/usr/lib/x86_64-linux-gnu/libhighs.so`
- `libhighs.dylib`
- `libhighs.so`

</li>
<li>
<b class="tab-title">MibS</b>
If you installed `idol` using a package manager (`apt-get` or `brew`), MibS should already be correctly configured. Try reinstalling `idol`.

If you built `idol` from source, make sure that MibS support was enabled during configuration:
- `USE_MIBS=ON`,
- `COIN_OR_DIR` must point to your coin-or installation directory (`dist/`).

If not, please refer to \ref build_from_source.

Also ensure that the coin-or `dist` directory was not moved after installation.
</li>
</ul>
</div>
