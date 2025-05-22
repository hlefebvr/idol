\page installation Installation guidelines

## Overview

Welcome to idol's installation guide! This document will help you set up idol on your system.

There are mainly to options to use idol. 

* The \subpage automatic-installation which lets CMake do the work for you and install idol automatically in your build directory.
* The \subpage manual-installation which manually installs idol from source and links it to your project.

The easiest is the first option, which is the recommended way to try idol.

Most likely, you will want to use idol in combination with external optimization solvers like Gurobi or HiGHS.
To do so, you need to have the solvers installed on your system and set the corresponding CMake options.
The \subpage cmake-options will help you to set the options correctly.
