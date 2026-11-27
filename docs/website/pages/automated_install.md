\page automated_install Automated Installation Script
\brief Build idol with MibS support in one step using the included script.

\tableofcontents

The repository includes `install_idol.sh`, which builds idol together with the
customized MibS fork in one step (the fork lets you use any idol-compatible
solver — such as Gurobi — as a feasibility checker inside MibS). Create a fresh directory, place the script
inside it, and run:

```shell
bash install_idol.sh
```

That's it. The script clones all dependencies, compiles everything in Release
mode using all available CPU cores, and installs idol into `idol/dist/` inside
that directory.

\section automated_path Making `idol_cl` available

Once the build finishes, add the binary to your PATH. Run `pwd` while still in
the build directory to get the absolute path, then substitute it below:

```shell
export PATH="/absolute/path/to/your/build/idol/dist/bin:$PATH"
```

To make this permanent, add that line to `~/.zshrc` (macOS) or `~/.bashrc`
(Linux). Then verify:

```shell
idol_cl --version
```

\section automated_prerequisites Prerequisites

You need: **bash**, **wget**, **git**, **CMake ≥ 3.22**, a **C++20 compiler**
(GCC or Clang), and **make**.

\note On macOS, `wget` is not installed by default. If you see "command not
found", install it first with `brew install wget`.

\section automated_troubleshooting Troubleshooting

\note The script has been tested on **macOS (arm64)**. On Linux it should work
too, but one internal patch may be silently skipped depending on your compiler
version. If the build fails during the MibS step, fall back to the
\ref cmake_mibs "manual MibS build".

Commercial solvers (Gurobi, CPLEX, …) are not included. Install them separately
and reconfigure using the \ref cmake_options "CMake options" on the
\ref build_from_source page.

---

For full control over compilers, optional dependencies and install location, see
\ref build_from_source.
