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

\section from_source From Source

See the \ref build_from_source page.
