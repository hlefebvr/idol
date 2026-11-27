\page build_from_source Manual Build from Source
\brief Compile and install idol from source with full control over build options.

\tableofcontents

This page covers building idol manually — useful when you need to control
compilers, CMake flags, optional dependencies or the install location.

For the quickest setup (idol + MibS in one command), see \ref automated_install instead.

\section requirements Requirements

To build `idol` and `idol_cl` from source, you need:

- **CMake ≥ 3.22**
- **C++20 compiler** (GCC or Clang)
- **Git**

You only need one C++ compiler — GCC (typical on Linux) or Clang (typical on
macOS via Xcode Command Line Tools). You can verify your tools with:

```shell
cmake --version
git --version
g++ --version      # GCC
clang++ --version  # Clang
```

\section build_steps Build instructions

\subsection step_clone Step 1 — Clone the repository

```shell
git clone https://github.com/hlefebvr/idol.git
```

\subsection step_build_dir Step 2 — Create a build directory

```shell
mkdir -p idol/build
cd idol/build
```

Using a separate build directory keeps the source tree clean.

\subsection step_configure Step 3 — Configure and build

```shell
cmake ..
cmake --build .
```

This compiles the `idol` library and the `idol_cl` executable.

\subsection step_install Step 4 — Install

```shell
sudo cmake --install .
```

By default, this installs files into:

```text
/usr/local/bin
/usr/local/lib
/usr/local/include
```

\subsection step_verify Step 5 — Verify installation

```shell
idol_cl --version
```

If the command prints the version, the installation was successful.

\section cmake_options CMake Options

You can customize the build by passing options to CMake.

\subsection cmake_mibs Enable MibS support

MibS is a mixed-integer bilevel solver that can be integrated in idol.

When installing via package managers (`apt-get` or `brew`), it is included automatically.
When building from source, you must build it separately and point CMake to it.

idol requires the customized fork
<a href="https://github.com/hlefebvr/MibS" target="_blank">hlefebvr/MibS</a>
(branch `idol`), not the upstream coin-or/MibS release. The fork enables using
any idol-compatible solver (e.g., Gurobi) as a feasibility checker inside MibS.
Build it using
<a href="https://github.com/coin-or/coinbrew" target="_blank">coinbrew</a>:

```shell
mkdir coin-or && cd coin-or
curl -fsSL https://raw.githubusercontent.com/coin-or/coinbrew/master/coinbrew -o coinbrew
chmod +x coinbrew
./coinbrew fetch https://github.com/hlefebvr/MibS@idol
./coinbrew build https://github.com/hlefebvr/MibS@idol --no-prompt --tests=none
cd ..
```

This installs into `coin-or/dist/`. Then either:

- set the environment variable:

```shell
export COIN_OR_HOME=/path/to/coin-or/dist
```

- or pass the directory directly to CMake with the option `COIN_OR_DIR`.

Finally, enable MibS with the CMake option `USE_MIBS=ON`.

```shell
cmake -DUSE_MIBS=ON -DCOIN_OR_DIR=/path/to/coin-or/dist ..
```

Both `COIN_OR_HOME` and `COIN_OR_DIR` must point to the COIN-OR `dist` directory.

\subsection cmake_cgl Enable Cgl support

Cgl is a cut generation library from COIN-OR that idol can use to generate
cutting planes in a branch-and-bound algorithm.

When installing via package managers (`apt-get` or `brew`), Cgl is included automatically.
When building from source, you must build it separately:

```shell
mkdir coin-or && cd coin-or
curl -fsSL https://raw.githubusercontent.com/coin-or/coinbrew/master/coinbrew -o coinbrew
chmod +x coinbrew
./coinbrew fetch Cgl@master
./coinbrew build Cgl
```

Then either set `COIN_OR_HOME` or pass `COIN_OR_DIR` to CMake (same as above),
and enable Cgl:

```shell
cmake -DUSE_CGL=ON -DCOIN_OR_DIR=/path/to/coin-or/dist ..
```

\subsection cmake_install_prefix Custom installation directory

To install into a custom location instead of `/usr/local`:

```shell
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
```

The executable will be installed into:

```text
$HOME/.local/bin/idol_cl
```

\subsection cmake_examples Build examples

```shell
cmake -DBUILD_EXAMPLES=ON ..
```

\subsection cmake_tests Build tests

```shell
cmake -DBUILD_TESTS=ON ..
```
