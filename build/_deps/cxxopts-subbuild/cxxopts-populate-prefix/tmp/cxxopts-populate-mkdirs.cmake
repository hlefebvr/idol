# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-src")
  file(MAKE_DIRECTORY "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-src")
endif()
file(MAKE_DIRECTORY
  "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-build"
  "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix"
  "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/tmp"
  "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp"
  "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src"
  "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/henri/Research/solvers/idol/build/_deps/cxxopts-subbuild/cxxopts-populate-prefix/src/cxxopts-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
