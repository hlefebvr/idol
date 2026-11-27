---
name: prepare-hpc-validation
description: Prepare an independent external HPC campaign handoff for scientific validation of a specific idol revision.
---

# Prepare hpc validation

Follow `AGENTS.md` throughout.

Large validation campaigns live in a separate repository. Do not implement campaign infrastructure in idol or submit HPC jobs without explicit authorization. This skill defines a handoff, not a claim of validation.

1. Identify exact candidate commit (or clearly label dirty/uncommitted changes), baseline commit, build flags, compiler, dependencies, solver versions/licenses, algorithm configuration, seeds and resource limits. Define the changed mathematical contract and failure risks.
2. Select relevant instance families and sizes: small analytic cases, representative cases, stress cases, degenerate/infeasible/unbounded cases when meaningful. For B&P use decomposable instances with diverse block sizes and structures, direct MILP references, and pricing/branching variants. For robust optimization include uncertainty-set types, recourse structure, feasibility and separation challenges; for bilevel include follower semantics. Choose independent instance sources and known reference results, not exclusively agent-generated fixtures.
3. Define independent checks: original-space primal feasibility and recomputed objective; valid bound direction and reference consistency; status/reason and gap; certificates where verifiable. Use reference formulations/solvers or analytic solutions independent of the candidate algorithm. A matching objective alone is insufficient; when no trusted optimum exists, report what can actually be established. Distinguish inconclusive timeout and unsupported cases from passing cases.
4. Define acceptance before running: no unexplained correctness violations; explicit numerical criteria consistent with configured tolerances; predefined coverage and handling of missing/error results. Performance regressions tracked separately using identical resource allocations, multiple seeds/runs when justified, and baseline comparisons. Do not invent universal pass thresholds; obtain maintainer agreement for campaign-specific thresholds.
5. Specify run metadata: instance ID/checksum, candidate/baseline SHA, build environment, solver settings, CPU/node/memory/thread budget, seed, run ID, exit code, status/reason, incumbent, bound, gap, independent check outcomes, wall time, memory and algorithm metrics (nodes, columns, scenarios, iterations as relevant). Preserve raw logs and reference provenance. Distinguish wall-time limits and external scheduler kills.
6. Supply a handoff with proposed matrix, independent checks, acceptance criteria, required output schema and unresolved dependencies. Link actual external results when available; record campaign ID and revision. Mark validation as pending until results have been reviewed. Passing finite campaigns validates the tested scope only, not every input or the mathematical proof.

Deliver: campaign specification suitable for the external benchmark repository; no fabricated results.
