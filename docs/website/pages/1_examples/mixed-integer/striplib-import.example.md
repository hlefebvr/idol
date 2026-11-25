\page example_mixed_integer_striplib_import striplib-import
\brief Imports a model and GCG decomposition in strIPlib format and solves it with idol's branch-and-price algorithm.

This example reads an LP model and an unpresolved `.dec` decomposition with
`DantzigWolfe::read_from_striplib`. It then composes
`DantzigWolfeDecomposition` with `BranchAndBound`, using GLPK for the restricted
master and pricing problems.

The files shipped with this example are small synthetic fixtures using the same
model and decomposition formats distributed by
[strIPlib](https://striplib.or.rwth-aachen.de/). Downloaded strIPlib archives
must be extracted before their model and decomposition paths are passed to the
reader.

\include ../examples/mixed-integer/striplib-import.example.cpp
