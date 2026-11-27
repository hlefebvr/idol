\page cli_installation Installation
\brief Describes how to install the command line interface `idol_cl`

\tableofcontents

`idol_cl` can be installed using a package manager (recommended) or built from source.

\section linux Linux (amd64)

Install using `apt-get`:

```shell
echo "deb [arch=amd64 trusted=yes] https://henrilefebvre.com/apt stable main" | sudo tee /etc/apt/sources.list.d/idol.list
sudo apt-get update
sudo apt-get install idol
idol_cl --version
```

\section mac MacOs (arm64)

Install using `brew`:
```cpp
brew tap hlefebvr/idol
brew install idol
idol_cl --version
```

\section cli_solver_config Solver Configuration

Once installed, solvers are configured via environment variables — no reinstallation needed. See \ref solver_configuration for the full reference.

\section from_source From Source

Two options are available:

- \ref automated_install "Automated script" — builds idol and the customized MibS fork in one command (the fork lets you use any idol-compatible solver as a feasibility checker inside MibS). The easiest way to get a full setup.
- \ref build_from_source "Manual build" — full control over compilers, CMake options, optional dependencies and install location.
