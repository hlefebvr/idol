---
name: review-mathematical-correctness
description: Audit mathematical validity, numerical behavior, termination, statuses, bounds and exactness of optimization code.
---

# Review mathematical correctness

Follow `AGENTS.md` throughout.

Perform an independent review of the change, not just a restatement of its author's reasoning.

- Identify the problem and claimed guarantees from the reference, implementation and API. Check assumptions are actually enforced or documented. Trace original-to-derived models and solution recovery; validate sense and objective constants.
- For every incumbent update ask whether the original problem is feasible; for every bound update derive why the bound is valid. Check min/max directions, initial sentinels, monotonicity within a solve where expected, and that an absolute gap does not conceal crossed bounds. Check proof conditions for pruning, convergence, infeasibility and unboundedness.
- Trace statuses and reasons through nested solves, limits, errors and missing certificates. Distinguish heuristic failure from proof. Inspect reduced-cost and Farkas conventions, exact fallback pricing/separation, and duality assumptions when relevant.
- Inspect configured feasibility/integrality/optimality/gap tolerances and sparsity, `idol::Inf`, comparisons, division, zero/negative objectives and scaling. Check unsupported cases fail explicitly and stale results are invalidated.
- For B&P: verify pricing certification, restricted master bounds, Farkas pricing, branch/cut validity and column reuse. For robust: verify scenario validity, recourse feasibility, adversarial bounds and complete separation requirements. For bilevel: verify follower optimality and optimistic/pessimistic semantics.
- Challenge the implementation with small adversarial or analytically known instances and an independent oracle. Distinguish verified defects from hypotheses. Do not silently rewrite the algorithm during a review.

Deliver findings ordered by severity with file/function, violated invariant, counterexample or reasoning, suggested fix and necessary regression. Say what was not verified. Passing examples are not a proof.
