\page build_from_source Build from source
\brief Describes how to compile and install idol from source.

\tableofcontents

\section requirements Requirements

To build `idol` from source, you need:

- **CMake ≥ 3.22**
- **C++20 compiler** (GCC or Clang)
- **Git**

You can verify your tools with:

```shell
cmake --version
g++ --version
clang++ --version
git --version
```

\section build_steps Build instructions

Follow these steps to compile and install `idol`.

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

You can customize the build using CMake options.


\subsection cmake_mibs Enable MibS support

When installing via package managers (`apt-get` or `brew`), MibS is  included automatically.  
When building from source, you must enable and configure it manually.

First, install MibS by following the instructions on the <a href="https://github.com/coin-or/MibS" target="_blank">official MibS documentation</a>. Then either:

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


\subsection cmake_install_prefix Custom installation directory

To install into a custom location, set:

```shell
cmake -DCMAKE_INSTALL_PREFIX=/custom/install/path ..
```

Example:

```shell
cmake -DCMAKE_INSTALL_PREFIX=$HOME/.local ..
```

The executable will be installed into:

```text
$HOME/.local/bin/idol_cl
```


\subsection cmake_examples Build examples

To build the example programs:

```shell
cmake -DBUILD_EXAMPLES=ON ..
```


\subsection cmake_tests Build tests

To build the test suite:

```shell
cmake -DBUILD_TESTS=ON ..
```
