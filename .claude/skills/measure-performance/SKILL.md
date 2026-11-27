---
name: measure-performance
description: Profile and measure a performance bottleneck before adding complexity to idol.
---

# Measure performance

Follow `AGENTS.md` throughout.

1. Define the workload, bottleneck hypothesis and relevant metric (total wall time, memory, model construction, update cost, pricing/separation time, node count, etc.). Use a reproducible baseline commit and configuration. Ensure baseline correctness first.
2. Profile representative workloads using available tools and targeted timers/counters. Separate solver time from framework overhead and exclude setup only if explicitly measured separately. Check allocations, copies, row/column materialization, rebuilding and repeated solver work. Avoid speculative micro-optimizations.
3. Implement the simplest candidate improvement in an isolated change. Preserve algorithmic semantics, valid bounds, tolerances, ownership, interoperability and lifecycle. Keep the main loop readable. If a less readable implementation is necessary, isolate it and document why.
4. Compare baseline and candidate under identical build flags, solver versions/settings, hardware, seeds, time/thread budgets and instances. Repeat when variability matters. Report sample size, distribution or variability, absolute and relative effect, memory and correctness checks. Do not claim significance from a single noisy timing. Use the external HPC campaign for substantial performance claims when appropriate.
5. Decide whether the measured benefit justifies complexity; there is no universal percentage threshold. If evidence is insufficient, keep the clearer implementation or report the optimization as unvalidated. Do not trade correctness for speed.

Deliver: bottleneck evidence, reproducible measurement setup, before/after data, correctness regression results and tradeoff rationale.
