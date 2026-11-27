---
name: implement-solver-wrapper
description: Add or change an external solver adapter, translating model operations, parameters, status and certificates.
---

# Implement solver wrapper

Follow `AGENTS.md` throughout.

1. Inspect `lib/include/idol/general/optimizers/Optimizer.h`, `lib/include/idol/mixed-integer/optimizers/wrappers/`, the corresponding `lib/src/` wrappers (e.g. Gurobi and HiGHS), `lib/CMakeLists.txt`, `cmake/`, and `tests/mixed-integer/wrappers/`. Read the external solver API for the installed/supported version; do not guess status codes or capabilities.
2. Build a capability matrix: model classes, objective senses, incremental operations, parameters, solution pool, primal/dual/reduced costs, rays/Farkas, callbacks and cancellation. Unsupported features must fail clearly, not silently degrade. Preserve optional dependency builds and do not force a proprietary solver on unrelated targets.
3. Map model-local indices to solver handles safely; account for removal/reindexing, objective constants, row/column representations, bound infinities, coefficient signs and type changes. Keep update notifications and solver state synchronized. Prefer incremental modifications; invalidate/rebuild lazily only when necessary. Manage native handles and callbacks with exception-safe ownership.
4. Implement status/reason translation for optimal, feasible, infeasible, unbounded, ambiguous and limit/error outcomes, checking actual incumbent and certificate availability. Validate sign and sense conventions independently. Never return a fake dual, zero ray or objective bound. Respect runtime parameters and documented solver defaults; reject unsupported settings where needed.
5. Exercise common LP/MIP wrapper tests, including min/max, nonzero constants, negative objectives, infeasibility, unboundedness, limits, edits/removals, repeated optimization and supported callbacks. Compare primal feasibility and dual/certificate properties against a small reference problem or another capable solver. Build with dependency enabled and disabled where possible; report missing SDK/licenses explicitly.

Deliver: capability/limitation summary, translation decisions, tested solver version and build flags, commands and results.
