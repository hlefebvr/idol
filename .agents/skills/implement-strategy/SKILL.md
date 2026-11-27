---
name: implement-strategy
description: Add or modify interchangeable algorithm strategies, callbacks, branching, pricing, stabilization or separation.
---

# Implement strategy

Follow `AGENTS.md` throughout.

1. Locate the relevant extension point before adding a class. Inspect `lib/include/idol/mixed-integer/optimizers/branch-and-bound/` (branching and node-selection `factories/` and `impls/`), `callbacks/Callback.h`, Dantzig-Wolfe stabilization/infeasibility strategies, or robust CCG separation. Read both factory and runtime invocation sites.
2. Specify inputs, outputs, ownership, callback context, legal actions, state/reset requirements and exact versus heuristic guarantees. Follow existing factory/implementation patterns; use a strategy rather than a complete optimizer unless it independently solves models.
3. Avoid coupling to a particular solver or concrete parent optimizer beyond the extension contract. Use existing capabilities and explicit compatibility checks. Keep configuration separate from mutable runtime state. Cloning must isolate state where required. Do not retain invalid node/model/solution references across callbacks.
4. Ensure a heuristic cannot accidentally trigger proof-based termination. For branching verify coverage of the feasible set, for cuts validity, for pruning valid bounds, for pricing reduced-cost semantics, and for separation correct violation/certification semantics. Respect resource budgets and legal callback operations; avoid reentrant solving unless supported.
5. Add focused unit tests plus a parent-algorithm integration test with the strategy enabled and an existing alternative. Cover no-op/empty outcomes, limits, resets and invalid configuration. Test deterministic behavior where appropriate; record seeds if randomness is intentional.

Deliver: extension contract, configuration example, mathematical role, integration results and unsupported contexts.
