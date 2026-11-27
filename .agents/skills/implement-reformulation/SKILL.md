---
name: implement-reformulation
description: Implement or change a mathematical transformation, relaxation, dualization or reformulation.
---

# Implement reformulation

Follow `AGENTS.md` throughout.

1. Inspect modeling `Model.h`, `Env.h`, annotations, expression types and nearby transformations (e.g. `lib/include/idol/bilevel/optimizers/StrongDuality/`, KKT, robust convexification, Dantzig-Wolfe). Determine whether the task needs a reusable transformation, an optimizer factory that solves the transformed model, or both.
2. Specify the original and transformed problems, assumptions, objective sense and whether the result is equivalent, a relaxation, restriction, or heuristic approximation. Derive the mapping and valid bound direction; account for constants, domains, empty sets, feasibility, unboundedness, duality assumptions and finite big-M validity. For bilevel/robust transformations state follower/recourse and uncertainty semantics.
3. Preserve object identity for unchanged objects within the same `Env`; create explicit maps for introduced variables and constraints. Do not use variable names as identity. Keep the parent model unchanged unless explicitly intended. Use existing expressions and descriptions. Read model-specific properties correctly.
4. Provide a clear recovery path for original-space primal and, where mathematically defined, dual information or certificates. Distinguish internal objectives from valid original bounds. Expose transformed-model solver choice via factory if the component solves it. Respect supported edits and lazy invalidation.
5. Test against direct formulations or analytically solved examples. Include infeasible/unbounded cases, nonzero constants, objective senses, mixed signs, boundary assumptions and unsupported input. Verify recovered primal feasibility independently and correct bound direction for relaxations; test repeated updates when applicable.

Deliver: derivation/reference, assumptions, mapping contract, test comparisons and limitations.
