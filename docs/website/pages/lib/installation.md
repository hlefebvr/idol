\page lib_installation Installation
\brief Describes how to install the C++ library `idol`

\tableofcontents

`idol` can be installed using a package manager (recommended) or built from source.
Then, to be used from C++, it requires to be linked to your code. 

Below, we give several examples on how to do this with the <a href="https://cmake.org/" target="_blank">CMake</a> build system.

\section lib_install Installation

\subsection lib_install_linux Linux (amd64)

Install using `apt-get`:

```shell
echo "deb [arch=amd64 trusted=yes] https://henrilefebvre.com/apt stable main" | sudo tee /etc/apt/sources.list.d/idol.list
sudo apt-get update
sudo apt-get install idol
idol_cl --version
```

\subsection lib_install_mac MacOs (arm64)

Install using `brew`:
```cpp
brew tap hlefebvr/idol
brew install idol
idol_cl --version
```

\subsection lib_install_from_source From Source

See the \ref build_from_source page.

\section lib_linking Linking with `idol` using CMake

Because `idol` is built with CMake, integrating it with other CMake projects is very simple. 

In your `CMakeLists.txt`, first look for the `idol` library by adding 

```cmake 
find_package(idol REQUIRED)
```

Then, assuming your target is called `my_target`, simply add 

```cmake
target_link_libraries(my_target PUBLIC idol)
```

That's it! You can now use `idol` in your C++ project.

\section lib_install_cmake Automatic Download using CMake `FetchContent`

Another possibility to link with `idol` is to let CMake download it and handle the linkage automatically. 

Let's assume that you have a basic CMake project with a `CMakeLists.txt` that looks like this:

```cmake 
cmake_minimum_required(VERSION 3.23)
project(my_project)

add_executable(my_target main.cpp)
```

This file creates a target called `my_target` made of only one source file `main.cpp`.

Next, let's see how to download `idol` and link it to your target.

First, we use `FetchContent` to download `idol`.

```cmake
include(FetchContent)

FetchContent_Declare(
        idol
        GIT_REPOSITORY https://github.com/hlefebvr/idol.git
        GIT_TAG        origin/main # Here, you can put a specific tag (e.g., v1.0.0) or a commit hash
)

# Ask CMake to download idol and install it to your build directory
FetchContent_MakeAvailable(idol)
```

Then, we can link `idol` to our target.

```cmake
target_link_libraries(my_target PUBLIC idol)
```

That's it! You can now use `idol` in your project.

> Note that you some CMake options are available and listed on the \ref build_from_source page.
>
> These can be set using `set(MY_OPTION VALUE)`. Be sure to check them out!
