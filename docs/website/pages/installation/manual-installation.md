\page manual-installation Installation from source

This section describes how to install idol from source using CMake and Make.
You may consider this as a more advanced option, but it is still very easy to do.

## Installation

First, we need to download idol from GitHub.

```shell
git clone https://github.com/hlefebvr/idol.git
cd idol
```

Then, we follow the standard CMake build procedure.

```shell
mkdir build
cd build
cmake ..
make
```

If there are no errors, we can install idol using the following command.

```shell
sudo make install
```

That's it! idol is now installed on your system. The next section describes how to link idol to your project.

> Note that you probably want to use idol in combination with external optimization solvers like Gurobi or HiGHS.
> To do so, you need to install the solvers and set the corresponding CMake options.
> See the [CMake Configuration Options](#cmake-configuration-options) section for more details.
>
> A CMake option can be set using `-DMY_OPTION=VALUE`, e.g., `cmake -DUSE_GUROBI=ON ..`.

If you do not have sudo privileges, you can also install idol in a custom folder.
To do so, use the standard `CMAKE_INSTALL_PREFIX` option to change the destination folder as follows:

```shell
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install ..
```

## Linking

Now that idol is installed, we can link it to our project.
To do so, we will assume that you have a basic `CMake` project with a `CMakeLists.txt` that looks like this:

```cmake
cmake_minimum_required(VERSION 3.23)
project(my_project)

add_executable(my_target main.cpp)
```

This file creates a target called `my_target` made of only one source file `main.cpp`.
Now, we show how to link idol to your target.

First, we need to find idol using the `find_package` command.

```cmake
find_package(idol REQUIRED)
```

Then, we can link idol to our target.

```cmake
target_link_libraries(my_target PUBLIC idol)
```

That's it! You can now use idol in your project.
