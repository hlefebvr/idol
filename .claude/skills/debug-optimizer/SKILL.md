---
name: debug-optimizer
description: Diagnose incorrect optimization results, bounds, crashes, stale state or numerical regressions.
---

# Debug optimizer

Follow `AGENTS.md` throughout.

1. Record failing revision, configuration, solver, seed, model, status/reason, expected behavior and actual result. Reproduce using the smallest case that preserves the issue. Preserve original evidence and distinguish reproducible failures from hypotheses.
2. Compare with an independent formulation, analytic solution or known reference. Inspect original-space feasibility and objective independently; inspect bound validity, not only objective agreement. Isolate modeling vs wrapper vs algorithm vs strategy failure by substitution where compatible.
3. Trace the lifecycle: object identity/environment, factory cloning, model edits, update hooks, reset/invalidation, child solver settings and result mapping. Compare a repeated edited solve with a fresh model. Examine termination and numerical comparisons; use logs or targeted debugger/instrumentation. For crashes use available sanitizers/debug builds where feasible.
4. Form a specific hypothesis and test it before changing behavior. Fix the root cause in the smallest appropriate scope. Do not hide failures using relaxed tolerances, ignored exceptions, arbitrary big-M, premature convergence or full rebuilds without justification. Report unrelated findings separately.
5. Add a deterministic regression that fails before the fix and passes after if practical. Run relevant existing tests and integration comparisons. For performance problems use `measure-performance`; for suspected invalid guarantees use `review-mathematical-correctness`.

Deliver: reproduction, root cause or remaining uncertainty, fix, regression evidence and remaining risks.
