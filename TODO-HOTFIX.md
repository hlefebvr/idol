# idol — Hotfix requests from benchmark

**Campaign:** `bap_bde0974036f6_fa909f04`  
**Benchmark:** 145 Striplib MILP instances, branch-and-price, 3600 s / 10 GB / 1 thread  
**idol SHA:** `bde0974036f68ff3114bd34cc01d6057a7bcf1bb` (codex branch)  
**Reported by:** hlefebvr/idol-benchmark, 2026-11-28  

---

## How to reproduce

### Instance files

All instances are LP+DEC pairs from the Striplib collection.

**GitHub (LFS):**
```
git clone https://github.com/hlefebvr/idol-benchmark
cd idol-benchmark
git lfs pull
# files are under: benchmarks/milp/striplib/instances/striplib_<name>.lp
#                                                       striplib_<name>.dec
```

**HPC direct path:**
```
isdm:/home/lefebvreh/Research/solvers/idol-benchmark/instances/milp/striplib/striplib_<name>.lp
isdm:/home/lefebvreh/Research/solvers/idol-benchmark/instances/milp/striplib/striplib_<name>.dec
```

### Minimal C++ reproducer skeleton

Each issue below can be reproduced with a small program linked against idol + CPLEX:

```cpp
#include <idol/modeling.h>
#include <idol/mixed-integer/modeling/read_from_striplib.h>
#include <idol/mixed-integer/optimizers/wrappers/Cplex/Cplex.h>
#include <idol/mixed-integer/optimizers/dantzig-wolfe/decomposition/decompositions/DantzigWolfe.h>
#include <idol/mixed-integer/optimizers/presets/BranchAndPriceBare.h>

using namespace idol;

int main(int argc, char* argv[]) {
    // argv[1] = path to .lp, argv[2] = path to .dec
    Env env;
    Model model = Cplex::read_from_file(env, argv[1]);
    Annotation<unsigned int> decomp = DantzigWolfe::read_decomposition(model, argv[2]);

    auto master  = Cplex::ContinuousRelaxation().with_logs(true).with_thread_limit(1);
    auto pricing = Cplex().with_logs(true).with_thread_limit(1);

    auto alg = Presets::BranchAndPriceBare(decomp, master, pricing)
                   .with_logs(true)
                   .with_time_limit(120)
                   .with_tol_mip_relative_gap(1e-4);
    model.use(alg);
    model.optimize();

    std::cout << "status: " << model.get_status() << "\n";
    std::cout << "reason: " << model.get_reason() << "\n";
}
```

Compile against idol (codex branch) and CPLEX 22.2. Set `CPLEX_STUDIO_DIR` or
`IDOL_CPLEX_PATH` to the CPLEX installation.

---

## HF1 — `"Ray not available."` exception crashes solver (9 instances)

### Status after codex commit 91da7593 ("HF1 real fix")

**Partially fixed.** The fix resolved 3 of the 9 instances
(`nrp_sprint06`, `nrp_sprint_late03_p`, `train-timetabling_cycle-T15S8L8Z1W1P3`).
The remaining 6 (`job-shop-scheduling_*`) still crash for a **different root cause**
(see HF1b below). Do not close HF1 until all 9 are resolved.

### Affected instances — all crash in < 1 s

```
FIXED   nrp_sprint06                nrp_sprint_late03_p
FIXED   train-timetabling_cycle-T15S8L8Z1W1P3

OPEN    job-shop-scheduling_abz5    job-shop-scheduling_ft06
OPEN    job-shop-scheduling_la25    job-shop-scheduling_la27
OPEN    job-shop-scheduling_la35    job-shop-scheduling_orb08
```

---

## HF1b — `"Ray not available."` in MIP pricing sub-problem (6 instances, still open)

### Affected instances

```
job-shop-scheduling_abz5    job-shop-scheduling_ft06    job-shop-scheduling_la25
job-shop-scheduling_la27    job-shop-scheduling_la35    job-shop-scheduling_orb08
```

### Reproduce

```bash
./reproducer striplib_job-shop-scheduling_abz5.lp \
             striplib_job-shop-scheduling_abz5.dec
# Observed: std::exception thrown: "Ray not available."  exit 1 in < 0.1 s
```

### Root cause

**File:** `lib/src/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.cpp`,
`save_infeasibility_information()`

The job-shop decomposition has **MIP pricing sub-problems** (the original LP contains
integer variables; each block inherits them). When a pricing sub-problem returns
`Unbounded`, `save_infeasibility_information` guards the ray extraction with:

```cpp
// Optimizers_Cplex.cpp: save_infeasibility_information
if (s == Unbounded && !m_is_mip && !m_has_quadratic_objective) {
    // ... CPXgetray ...
    m_primal_ray.emplace(...);
}
// m_is_mip == true for job-shop sub-problems → branch never taken
// m_primal_ray stays empty
```

`CPXgetray` only works on LPs; for MIPs CPLEX has no equivalent.
`m_primal_ray` is therefore never set, and the subsequent `save_ray()` →
`get_var_ray()` call throws immediately.

The info re-solve added in commit 91da7593 (ColumnGeneration.cpp) correctly
mirrors the master re-solve pattern and gates entry on the status check, but
it cannot help because `save_infeasibility_information` exits early for MIPs
before ever calling `CPXgetray`.

### Fix (implemented)

`Cplex::save_infeasibility_information` now handles the MIP case by flipping
the problem type to LP (`CPXchgprobtype(idol_CPXPROB_LP)`), re-solving as LP
(primal simplex for `Unbounded`, dual simplex for `Infeasible`), extracting
the ray / Farkas certificate on the LP state, and restoring
`idol_CPXPROB_MILP`. CPLEX preserves per-variable integrality across the
probtype flip, so the sub-problem keeps its MIP shape.

If `CPXgetray` / `CPXdualfarkas` still fails (ray numerically unavailable, or
the LP relaxation of an infeasible MIP is itself feasible), the corresponding
field is reset so the caller reports a clean missing-certificate state rather
than a crash.

---

## HF1c — DEC constraint name truncated at `;` by CPLEX LP reader (4 instances)

**Owner: idol** — the LP reader (via CPLEX) silently truncates constraint names
at `;`, so the full names in the DEC file are never found.

### Affected instances — all crash immediately on decomp read

```
cpmp_p2350-1.txt
cpmp_p33100-12.txt
cuttingstock_TEST0028-18
cuttingstock_TEST0056-15
```

Exact error:
```
exception: Unknown constraint "b_capa_;1;" in decomposition file.
```

### Reproduce

```bash
# Local instance files:
# /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/

./reproducer \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_cpmp_p2350-1.txt.lp \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_cpmp_p2350-1.txt.dec
# Observed: exception at DantzigWolfe::read_decomposition — "Unknown constraint b_capa_;1;"
# Crashes before any optimization starts.
```

### Root cause

The DEC file lists constraint `b_capa_;1;` as a block member. When CPLEX reads
the LP file, it truncates constraint names at `;` (treats it as a terminator in
its LP format parser), so the constraint is stored internally as `b_capa_`. The
DEC reader then looks up `b_capa_;1;` in idol's model and fails to find it.

```
LP file  → CPLEX reads → model stores name as: "b_capa_"
DEC file → reads name:                          "b_capa_;1;"  ← not found → throws
```

### Fix (implemented)

`read_from_striplib.cpp` now resolves DEC constraint names with a semicolon
fallback: if the full name is not found in the model, the substring before
the first `;` is tried. Matches are then routed through the standard
block/master assignment. A regression test (`semicolon-truncated-name.dec`)
checks that a DEC entry like `block_1;1;` resolves to the LP-reader-stored
`block_1`.

---

## HF1d — Unknown DEC constraint name due to LP/DEC naming mismatch (3 instances)

**Owner: idol** — the DEC was generated against a different naming convention
than what CPLEX uses when it reads the LP. Rather than hard-failing, idol should
emit a warning and treat the constraint as a master constraint.

### Affected instances — crash immediately on decomp read

```
hlp_ap25_c_p4_a05
hlp_tr10_c_p4_a05
hlp_tr40_c_p4_a05
```

Exact error:
```
exception: Unknown constraint "demand_0_1_inhub_0" in decomposition file.
```

### Reproduce

```bash
./reproducer \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_hlp_ap25_c_p4_a05.lp \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_hlp_ap25_c_p4_a05.dec
# Observed: exception at read_decomposition — "Unknown constraint demand_0_1_inhub_0"
```

### Root cause

The constraint `demand_0_1_inhub_0` appears in the DEC file but not in idol's
model after CPLEX reads the LP. The LP likely names the constraint differently
(e.g., `c1`, auto-generated, or truncated). The DEC was produced by a tool that
read the LP with a different parser (GCG/SCIP), which preserves a different name.

### Fix (implemented)

`read_from_striplib.cpp` now emits a warning and skips unknown DEC constraint
names instead of throwing. Skipped names remain implicitly in the master
(since the `decomposition` annotation defaults to `MasterId` for unassigned
objects). The existing `modeling.striplib` "unknown constraint is tolerated
with a warning" test case guards this behaviour.

---

## HF1e — DEC duplicate constraint name causes hard failure (2 instances)

**Owner: idol** — a repeated constraint name in the DEC should warn and skip,
not throw.

### Affected instances — crash immediately on decomp read

```
clp_BR2_42
clp_BR3_73
```

Exact error:
```
exception: Cannot read a name-based decomposition: duplicate constraint name "orthogonalityb(0)".
```

### Reproduce

```bash
./reproducer \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_clp_BR2_42.lp \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_clp_BR2_42.dec
# Observed: exception at read_decomposition — duplicate constraint name
```

### Root cause

The DEC file assigns `orthogonalityb(0)` to two different blocks.
idol's DEC reader throws immediately on the second assignment.

### Fix (implemented)

Both the model-side `objects_by_name` helper (duplicate row/column names in
the imported LP) and the DEC-side `assign` helper (duplicate block assignments
for the same name) now warn and keep the first occurrence instead of throwing.
Lets the decomposition be read on LPs that CPLEX accepts with duplicate row
names and on DEC files that double-assign a constraint.

---

## HF1f — `boundsL` / `regionGe` constraints in DEC not in LP (3 instances)

**Owner: idol** — GCG encodes variable bounds as named constraints
(`boundsL(i)`, `regionGe(i,j)`) in DEC files. The LP reader adds bounds as
variable attributes, not as named constraints. The DEC reader should recognise
these patterns as bound constraints and handle them gracefully.

### Affected instances — crash immediately on decomp read

```
clp_BR2_41    →  "Unknown constraint "boundsL(0)" in decomposition file."
clp_BR3_12    →  "Unknown constraint "boundsL(0)" in decomposition file."
clp_BR3_42    →  "Unknown constraint "regionGe(0,1)" in decomposition file."
```

### Reproduce

```bash
./reproducer \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_clp_BR2_41.lp \
  /Users/henri/Research/Solvers/idol-benchmark/benchmarks/milp/striplib/instances/striplib_clp_BR2_41.dec
# Observed: exception — "Unknown constraint boundsL(0)"
```

### Root cause

GCG writes `CONS` sections in DEC files using synthetic constraint names
for variable bounds:
- `boundsL(i)` = lower bound `x_i ≥ lb_i`
- `regionGe(i,j)` = a regional bound constraint

The LP format stores these bounds as variable attributes (`lb ≤ x ≤ ub`),
not as named rows. When idol reads the LP through CPLEX, no constraint named
`boundsL(0)` exists in the model.

### Fix (implemented)

Covered by the HF1d fix: unknown DEC constraint names (including the
synthetic `boundsL(i)` / `regionGe(i,j)` entries that GCG emits for
variable bounds) now warn and are skipped. Variable-bound constraints are
already enforced on the model variables, so skipping them is the correct
behaviour.

---

## HF2 — Column generation does not converge at root node (26 instances)

### Affected instances — all: nodes = 1, dual bound = null, wall time = 3600 s

```
bpp_N2W3B1R9.BPP               bpp_N3W1B3R6.BPP               bpp_N4W2B3R4.BPP
cutpacking_graph_20_40_1        cutpacking_graph_40_20_1
gap_c05200_1.txt
hlp_cab35_c_p4_a05
maplabeling_d1000_16_alternative
nrp_long_hint02
pcp_n120p5t2s2                  pcp_n90p4t2s1
relaxed-clique_4-club_partitioning_adjnoun
tup_umps14A_7_3                 tup_umps14B_5_3                tup_umps16C_8_4
tup_umps16_7_3                  tup_umps16_8_2
uc_100_0_3_w                    uc_150_0_4_w                   uc_200_0_5_w
uc_50_0_5_w
umcfp_Bs21
vcol_DSJC125.1.col
vrp_10G1.cvrp                   vrp_59G1.cvrp                  vrp_F-n45-k4.cvrp
```

These are **not hard instances** — Coluna (B&C&P) solves representative overlaps:

| Instance | idol (bare) | idol (Wentges ρ=0.4) | Coluna |
|---|---|---|---|
| `nrp_long_hint02` | 3600 s, 1 node, no bound | 3600 s, 1 node, no bound | 43 s ✓ |
| `tup_umps14A_7_3` | 3600 s, 1 node, no bound | 3600 s, 1 node, no bound | 37 s ✓ |
| `pcp_n120p5t2s2`  | 3600 s, 1 node, no bound | 3600 s, 1 node, no bound | 53 s ✓ |

**Wentges stabilisation (ρ=0.4) produces the same stall on all 26 instances.**

### Reproduce

```bash
./reproducer striplib_nrp_long_hint02.lp striplib_nrp_long_hint02.dec
# Observed: runs for --time-limit seconds, exits with status=Unknown,
#           nodes=1, best_bound=null.  Enable .with_logs(true) to see CG output.
```

### Root cause

**File:** `lib/src/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.cpp`,
`check_stopping_criterion()` (~line 246)

```cpp
// ColumnGeneration.cpp:258-260
if (m_parent.get_remaining_time() <= 0) {
    m_reason = TimeLimit;
    return true;
}
```

The time-limit check fires only **between** pricing iterations. With no
iteration cap (`Inf` by default), if each pricing call succeeds but the master
LP optimality gap never closes, the loop runs until the clock expires without
ever yielding a valid dual bound. The B&B tree is therefore never entered (1 node).

### Fix (implemented)

The stall was caused by two interacting bugs in `ColumnGeneration`:

1. `analyze_sub_problems()` guarded the Lagrangian-bound update with
   `all_sub_problems_have_valid_bounds = m_current_pricing_uses_raw_dual`.
   That flag was `false` even when the stabilisation returned the master dual
   unchanged (`NoStabilization` always; `Wentges`/`Neame` on their first call
   or when `ρ ≈ 0`). The bound was therefore never computed in those cases,
   causing even bare CG to stall on these instances.

2. Under genuine Wentges smoothing, raw-dual pricing was only triggered by the
   misprice mechanism (smoothed pricing returns zero columns). When smoothed
   pricing kept finding tiny-reduced-cost columns forever, the bound was never
   refreshed.

Fix:

- Added `last_result_equals_master_dual()` to `DualPriceSmoothingStabilization::Strategy`.
  `NoStabilization` returns `true` always; `Wentges` and `Neame` track whether the
  last `compute_smoothed_dual_solution` call returned the master dual unchanged.
- In `update_sub_problems`, upgrade `m_current_pricing_uses_raw_dual` to `true`
  when the stabilisation's last result equals the master dual.
- Added a `m_iterations_since_bound_refresh` counter with period `10`. When no
  bound was computed for 10 iterations, `m_force_raw_pricing` is set so the next
  iteration uses raw duals and refreshes the Lagrangian bound.

The artificial `.with_max_root_cg_iterations(10'000)` from `BranchAndPrice.h`
and `BranchAndPriceBare.h` was removed. The public API
`with_max_root_cg_iterations()` is kept as a user-tunable knob.

---

## HF3 — Empty block in DEC throws instead of skipping (1 instance)

### Affected instance

```
vcol_gcol30.txt
```

### Reproduce

```bash
./reproducer striplib_vcol_gcol30.txt.lp striplib_vcol_gcol30.txt.dec
# Observed: std::exception: "Empty block 178 cannot be represented by
#            idol's current Dantzig-Wolfe formulation."
```

### Root cause

**File:** DEC parser / DW formulation builder (search for `"Empty block"`)

The DEC file contains 178+ blocks; block 178 has no variables or constraints.
GCG writes empty blocks in some decompositions (the block contributes only via
the implicit convexity constraint / `PricingSetupVar`).

### Fix (implemented, commit 892b332c)

`read_from_striplib.cpp`'s `read_decomposition` now emits a warning for empty
blocks instead of throwing. Empty sub-problems are mathematically valid: their
`PricingSetupVar` column (lb=1, ub=1) enforces the convexity row.

---

## HF4 — Uncaught CPLEX exception propagates out of B&P loop (1+ instances)

### Affected instance

```
lotsizing_F14N
```

Error recorded: `"CPLEX Error 1256: Basis singular."` — crash in < 1 s.

### Reproduce

```bash
./reproducer striplib_lotsizing_F14N.lp striplib_lotsizing_F14N.dec
# Observed: std::exception thrown, exit 1.  Error originates from CPLEX
#           returning a singular-basis status during LP re-solve.
```

### Root cause

CPLEX Error 1256 (singular basis) is thrown as a C++ exception by the CPLEX
wrapper but is not caught at the outer B&P solve boundary. Any recoverable
CPLEX numerical error (1256, 1265 — dual unbounded ray, and others) currently
causes the entire run to abort.

### Fix (implemented)

- Added `idol::NumericalException : public Exception` for recoverable solver
  numerical failures (distinct from semantic/user-error exceptions).
- The CPLEX wrapper's `check()` function now throws `NumericalException` on
  non-zero CPLEX error codes (covers Error 1256 singular basis, 1265 dual
  unbounded ray, and similar).
- `ColumnGeneration::execute()` wraps the main CG loop in a `try/catch (const
  NumericalException&)`. On catch, the node is marked `Unknown / Numerical` and
  `m_is_terminated = true`; B&B then prunes it and continues. Plain
  `idol::Exception` (semantic errors) still propagates to the caller.

---

## HF5 — B&P iteration count not accessible via public API

### What happens

The benchmark records `bp_iterations = null` for **all** runs (bare and
recommended, every status). The node count is successfully read via:

```cpp
const auto* bab = dynamic_cast<
    const Optimizers::BranchAndBound<NodeWithCGInfo>*>(&model.optimizer());
if (bab) nodes = (long)bab->n_created_nodes();   // works
```

No equivalent accessor exists for the total number of column-generation master
iterations across all B&B nodes. This metric is essential for comparing
stabilisation strategies and diagnosing HF2.

### Fix (implemented, commit e7e941ec)

`Optimizers::DantzigWolfeDecomposition` exposes
`n_cg_iterations()` (cumulative across all B&B nodes) and
`n_root_cg_iterations()` (root node only). Access via
`dynamic_cast<const Optimizers::DantzigWolfeDecomposition*>(...)` on the
B&P sub-optimizer. The counters are reset in `hook_before_optimize`.

---

## HF6 — No primal solution found on 50 instances despite extensive B&B

### What happens

50 instances time out (`status=Unknown`) with many B&B nodes explored but
`objective_value = null` — no feasible integer solution was ever found.
The B&B tree is fully explored but branching never produces a leaf with a
feasible LP/MIP solution. Node count distribution:

```
> 100 000 nodes :  3 instances   (maplabeling_d25_08_alternative: 858 903 nodes)
  10 000–100 000:  15 instances
   1 000–10 000 :  18 instances
     100–1 000  :  13 instances
      10–100    :   1 instance
```

Representative instances (bare config, 3600 s):

```
maplabeling_d25_08_alternative   nodes=858903, bound=28,         no primal
scheduling_U_3_0050_05_1         nodes=273571, bound=53504,      no primal
scheduling_U_1_0050_10_3         nodes=105665, bound=271,        no primal
cuttingstock_TEST0065-01         nodes= 69053, bound=8,          no primal
umcfp_Bs01                       nodes= 37915, bound=1575803,    no primal
vrp_instance3                    nodes= 19151, bound=619,        no primal
```

Families: `scheduling` (6), `fcnet` (5), `umcfp` (5), `cwlp` (4),
`cuttingstock` (3), `nrp` (3), `pcp` (3), `sslp` (3), `bpp` (2), and others.

For comparison: `cplex` (direct MIP) solves 115/149; `gcg` solves 71/149.

### Note

This is a **performance issue, not a correctness issue**. Pure branch-and-price
without a primal heuristic relies entirely on finding a naturally integer
LP solution at a leaf node, which can take an exponential number of nodes.

### Suggested investigation

1. **Diving heuristic at root or periodically during B&B** — fix fractional
   master columns greedily, resolve the sub-MIP, and check integrality. This
   is the standard method for getting early incumbents in B&P.

2. **LP-rounding heuristic** after root CG convergence — round the fractional
   master solution and re-solve pricing to verify feasibility.

3. **For the `scheduling` family** — verify that the DW decomposition's pricing
   MIP produces columns with the correct integrality structure at branching nodes.

---

## Reproduction environment

```
HPC:      isdm (LIRMM cluster, cpu-dedicated partition)
CPLEX:    22.2.0  (libcplex2220.so, dynamically loaded)
idol:     codex branch, SHA 56e013dc6f5d84450ae31e08357887a63909a204
Config bare:        Presets::BranchAndPriceBare — MostInfeasible / BestBound,
                    no stabilisation, 1 thread
Config recommended: Presets::BranchAndPrice   — same base + Wentges(ρ=0.4)
Time limit:         3600 s
Memory limit:       10 GB
Threads:            1
Rel. gap tolerance: 1e-4
```

All 9 HF1 crashes reproduce in < 1 s.  
All 26 HF2 stalls reproduce with any `--time-limit` ≥ 5 s (1 node, no bound).  
HF3 reproduces immediately (exception at decomposition-read time).  
HF4 reproduces in < 1 s.
