---
name: implement-optimizer
description: Implement or modify a complete idol optimizer and its factory, including lifecycle, composition and mathematical contracts.
---

# Implement optimizer

Follow `AGENTS.md` throughout.

1. Read `AGENTS.md`, the relevant paper or specification, `Optimizer.h`, `Algorithm.h`, `OptimizerFactory.h` in `lib/include/idol/general/optimizers/`, and `lib/src/mixed-integer/general/OptimizerFactory.cpp`. Inspect `Model.h` and a structurally similar optimizer, its example and tests. For decomposition inspect `DantzigWolfeDecomposition.h`; for nested algorithms inspect `BranchAndBound.h` or robust `ColumnAndConstraintGeneration.h` (find under `lib/include/idol/`).
2. Write down problem class, assumptions, sense, exactness, input description, supported modifications, invariants, bound derivation, termination and required child capabilities. Identify ambiguities before coding; do not invent mathematical requirements.
3. Design a public factory with existing fluent configuration conventions and a separate model-bound execution object. Follow neighboring factory/implementation namespace patterns, `create`, `clone`, copy semantics and ownership. Use existing default parameter CRTP where appropriate. Inject suboptimizer factories and strategy factories; do not hardcode solvers. Verify factory copies own their configuration and no temporary references escape.
4. Implement the main loop in recognizable pseudocode order using focused named functions. Use parent model identity, annotations and explicit mappings where new objects are introduced. Preserve parent model semantics. Implement applicable build/update hooks and state invalidation; prefer incremental or partial maintenance and lazy full rebuild only when necessary. Handle repeated solves and queries after invalidation.
5. Handle status, reason, primal, bounds, gaps and certificates in parent coordinates. Reject unsupported operations and capabilities explicitly. Respect tolerances, sense, remaining time, termination and thread limits. Retain valid results on early termination.
6. Register source and public headers in the existing build structure as needed. Add a minimal deterministic test and example, and tests for exactness, limits, incompatible capabilities and repeated edits. Run the review and interoperability skills. Prepare external campaign scope when the feature affects scientific results.

Deliver: a concise mathematical contract, changed API/configuration, implementation and test results, known limitations and validation status. Do not claim exactness merely because a reference instance matches.
