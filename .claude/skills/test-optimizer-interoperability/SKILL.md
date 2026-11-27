---
name: test-optimizer-interoperability
description: Test optimizer composition, factory cloning, parameter propagation, incremental model updates and solver substitution.
---

# Test optimizer interoperability

Follow `AGENTS.md` throughout.

1. Inspect `tests/CMakeLists.txt`, domain CMake files, `tests/mixed-integer/{modeling,wrappers,branch-and-bound,branch-and-price}/`, and examples such as `gap-bap.example.cpp` and `flp-ccg.example.cpp`. Reuse existing test framework and target conventions; do not assume all tests are registered with CTest.
2. Identify mathematical capability requirements and select compatible available backends. Test the factory independently and as a child optimizer. Compare compositions with an independent direct solver/formulation or analytic result. Do not assume every optimizer supports all model types or certificates.
3. Test copy/clone/reuse: create multiple models from a factory, configure independent copies, solve sequentially and verify no leaked mutable state. Test temporary factory lifetimes as supported by the API. Check annotation/object identity across model copies.
4. Compare repeated solves and supported edits with freshly constructed equivalent models: variable bounds/types/objectives, RHS, matrix coefficients, adds/removes, sense/constant, and supported quadratic/SOS. Test batched edits and invalidation without unnecessary full rebuilding. Explicitly verify unsupported operations are rejected.
5. Exercise parent time/thread/iteration limits, logging, tolerance precedence, cancellation and early termination. Check incumbent and valid bound independently, status/reason and solution availability; check duals/rays/Farkas only under valid conditions. Include min/max, infeasible, unbounded, degeneracy and negative/zero objective cases as applicable.
6. Configure from the repository root, e.g. `cmake -S . -B build-agent -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON` plus verified optional solver flags. Build relevant targets with `cmake --build build-agent --target <target> -j <budget>`. Inspect registration and use `ctest --test-dir build-agent --output-on-failure` only for registered tests; otherwise run actual executables from required working directories. Check test counts and exit codes. Report skipped backends and why.

Deliver: compatibility matrix and commands/results; separate compile, local, integration and external validation evidence. Never change expectations merely to fit the implementation.
