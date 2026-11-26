\page lib_milp_branch_and_price Branch-and-Cut-and-Price
\brief Describes how to create a branch-and-cut-and-price algorithm

\tableofcontents

\section lib_milp_branch_and_price_preset Recommended preset

`Presets::BranchAndPrice` provides idol's recommended branch-and-price
configuration. The caller supplies both solver factories; the preset does not
select or hardcode a backend.

```cpp
#include "idol/mixed-integer/optimizers/presets/BranchAndPrice.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

model.use(Presets::BranchAndPrice(
        decomposition,
        GLPK::ContinuousRelaxation(),
        GLPK()));
model.optimize();
```

The preset uses `NodeWithCGInfo`,
`MostInfeasible` branching, `BestBound` node selection, the existing
`DantzigWolfeDecomposition` defaults, and `Wentges(.4)` stabilization. It does
not install an integer-master heuristic or another callback. The returned
`BranchAndBound<NodeWithCGInfo>` remains configurable through its normal API:

```cpp
auto optimizer = Presets::BranchAndPrice(
        decomposition,
        GLPK::ContinuousRelaxation(),
        GLPK());
optimizer.with_time_limit(300.).with_logs(false);
model.use(optimizer);
```

\subsection lib_milp_branch_and_price_bare Explicit bare configuration

For experiments requiring an explicitly selected bare configuration, compose
the existing factories directly. The following configuration uses no dual-price
stabilization:

```cpp
const auto column_generation = DantzigWolfeDecomposition(decomposition)
        .with_master_optimizer(GLPK::ContinuousRelaxation())
        .with_default_sub_problem_spec(
                DantzigWolfe::SubProblem().add_optimizer(GLPK()));
const auto branch_and_bound = BranchAndBound<NodeWithCGInfo>()
        .with_branching_rule(MostInfeasible())
        .with_node_selection_rule(BestBound());

model.use(branch_and_bound + column_generation);
model.optimize();
```

Use explicit composition when changing Dantzig-Wolfe strategies or when a
stable experimental configuration must remain independent of later preset
changes.

\section lib_milp_branch_and_price_dantzig_wolfe Using the Dantzig-Wolfe Decomposition

\subsection lib_milp_branch_and_price_striplib Importing a strIPlib decomposition

[strIPlib](https://striplib.or.rwth-aachen.de/) distributes mixed-integer models
together with decompositions in GCG's `.dec` format. Download and extract an
instance archive from the strIPlib browser, then pass the extracted model and
decomposition files to `DantzigWolfe::read_from_striplib`:

```cpp
#include "idol/modeling.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/MostInfeasible.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestBound.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/DantzigWolfeDecomposition.h"
#include "idol/mixed-integer/optimizers/dantzig-wolfe/SubProblem.h"
#include "idol/mixed-integer/optimizers/wrappers/GLPK/GLPK.h"

using namespace idol;

Env env;
auto [model, decomposition] = DantzigWolfe::read_from_striplib(
        env, "instance.lp", "instance.dec");

const auto column_generation = DantzigWolfeDecomposition(decomposition)
        .with_master_optimizer(GLPK::ContinuousRelaxation())
        .with_default_sub_problem_spec(
                DantzigWolfe::SubProblem().add_optimizer(GLPK()));
const auto branch_and_bound = BranchAndBound()
        .with_branching_rule(MostInfeasible())
        .with_node_selection_rule(BestBound());

model.use(branch_and_bound + column_generation);
model.optimize();
```

The model file is read through `Model::read_from_file`, so at least one
installed solver wrapper capable of importing its format must be available.
Compressed files from a strIPlib archive must be extracted before they are
passed to this interface.

The importer preserves the model supplied by the model reader and translates
the constraint and variable names in the `.dec` file into an
`Annotation<unsigned int>`. Constraints declared under `MASTERCONSS` remain in
the master. Constraints declared under `BLOCK`, `BLOCKCONS`, or `BLOCKCONSS`
are assigned to zero-based idol subproblem identifiers even though GCG block
numbers start at one. Variables are inferred from block constraints, as in the
GCG format. `BLOCKVARS` and `MASTERVARS` declarations are also checked and
honored.

idol currently rejects presolved decompositions, empty blocks, nonlinear
models, and linking variables that occur in more than one block. These cases
cannot be represented faithfully by the current Dantzig-Wolfe formulation.
Unknown names, duplicate assignments, malformed sections, and unassigned
constraints when `CONSDEFAULTMASTER` is zero also produce an exception.

For research use, cite the instance's original source as indicated by strIPlib
and the library itself: M. Bastubbe, A. Helber, L. Kirchhart, M. Lübbecke,
N. Rieken, and J. Witt, *strIPlib: Structured Integer Programming Library*,
https://striplib.or.rwth-aachen.de/. The small files shipped with the idol
example are synthetic format fixtures, not redistributed strIPlib instances.

\section lib_milp_branch_and_price_bnb Branching

\section lib_milp_branch_and_price_advanced More Advanced Topics

\subsection lib_milp_branch_and_price_advanced_cg About Dantzig-Wolfe Decomposition

- \subpage lib_milp_branch_and_price_cg_phaseI
- \subpage lib_milp_branch_and_price_cg_in_out
- \subpage lib_milp_branch_and_price_cg_callback
- \subpage lib_milp_branch_and_price_cg_HeuristicPricing
- \subpage lib_milp_branch_and_price_cg_options

\subsection lib_milp_branch_and_price_advanced_bap About Branch-and-Cut-and-Price

- \subpage lib_milp_branch_and_price_bap_IntegerMaster

\page lib_milp_branch_and_price_cg_phaseI Making the Master Problem Feasible (Phase I)
\tableofcontents

\page lib_milp_branch_and_price_cg_in_out Stabilizing Column Generation via in-out Separation
\tableofcontents

\page lib_milp_branch_and_price_cg_HeuristicPricing Using Heuristics for Pricing
\tableofcontents

\page lib_milp_branch_and_price_cg_callback Using Your Own Pricing Callback
\tableofcontents

\page lib_milp_branch_and_price_bap_IntegerMaster The "Integer Master" Heuristic
\tableofcontents

\page lib_milp_branch_and_price_cg_options Other Options
\tableofcontents

\section lib_milp_branch_and_price_cg_options_parallel Parallel Pricing

\section lib_milp_branch_and_price_cg_options_multiplicities Multiplicities

\section lib_milp_branch_and_price_cg_options_clean_up Column Pool Clean Up
