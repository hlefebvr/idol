# idol — Hotfix requests from benchmark

**Campaign:** `bap_56e013dc6f5d`  
**Benchmark:** 149 Striplib MILP instances, branch-and-price, 3600 s / 10 GB / 1 thread  
**idol SHA:** `56e013dc6f5d84450ae31e08357887a63909a204` (codex branch)  
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

### Affected instances — all crash in < 1 s

```
job-shop-scheduling_abz5    job-shop-scheduling_ft06    job-shop-scheduling_la25
job-shop-scheduling_la27    job-shop-scheduling_la35    job-shop-scheduling_orb08
nrp_sprint06                nrp_sprint_late03_p
train-timetabling_cycle-T15S8L8Z1W1P3
```

### Reproduce

```bash
./reproducer striplib_job-shop-scheduling_abz5.lp \
             striplib_job-shop-scheduling_abz5.dec
# Observed: std::exception thrown: "Ray not available."  exit 1
```

### Root cause

**Files:**
- `lib/include/idol/mixed-integer/modeling/models/Model.h`, line ~376 (`save_ray`)
- `lib/src/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.cpp`, lines ~131–132 and ~294–296
- `lib/src/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.cpp`, line ~191 (`Cplex::get_var_ray`)

`save_ray` (Model.h) throws when the model's status is not `Unbounded`:

```cpp
// Model.h:369-392
static auto save_ray(const Model& t_original_model, const Model& t_model) {
    if (t_model.get_status() != Unbounded) {
        throw Exception("Ray not available.");   // ← crashes here
    }
    ...
}
```

`m_primal_ray` is populated in `Cplex::save_infeasibility_information` only
when `s == Unbounded && !m_is_mip`. After an internal LP re-solve, the CPLEX
status can shift; `m_primal_ray` remains empty and `save_ray` throws.

The stale status is read in `ColumnGeneration`:

```cpp
// ColumnGeneration.cpp:131-132  (master ray, using PRE-re-solve status)
if (m_status == Unbounded) {           // m_status set before re-solve at line ~92
    m_master_primal_ray = save_ray(master);   // master may no longer be Unbounded
}

// ColumnGeneration.cpp:294-296  (sub-problem ray)
if (model.get_status() == Unbounded) {
    auto ray = save_ray(model);               // same pattern
}
```

### Suggested fix

Re-read `master.get_status()` after the internal re-solve before calling `save_ray`:

```cpp
// ColumnGeneration.cpp, after the re-solve block (~line 96):
m_status = master.get_status();   // override stale pre-re-solve value
...
if (m_status == Unbounded) {
    m_master_primal_ray = save_ray(master);
}
```

Also guard `Cplex::save_infeasibility_information` against `CPXgetray` failure:

```cpp
// Optimizers_Cplex.cpp, inside save_infeasibility_information:
int rc = l.CPXgetray(m_env, m_model, m_primal_ray->data());
if (rc != 0) {
    m_primal_ray.reset();   // CPXgetray failed — do not mark ray as valid
}
```

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

### Suggested fix

Expose accessors on the B&P / DW optimizer:

```cpp
// e.g., on BranchAndBound<NodeWithCGInfo> or its DW sub-optimizer:
[[nodiscard]] long n_cg_iterations() const;        // total across all B&B nodes
[[nodiscard]] long n_root_cg_iterations() const;   // root node only (most useful for HF2)
```

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
