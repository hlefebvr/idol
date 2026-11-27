# idol — Hotfix requests from benchmark (56e013dc6f5d)

Reported by: hlefebvr/idol-benchmark, campaign bap_56e013dc6f5d, 2026-11-27.
Benchmarked on 149 Striplib MILP instances, 3600 s / 10 GB / 1 thread, CPLEX 22.2.

---

## HF1 — `"Ray not available."` exception crashes solver (9 instances)

**Files:**
- `lib/include/idol/mixed-integer/modeling/models/Model.h`, line 376 (`save_ray`)
- `lib/src/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.cpp`, lines 131–132 and 294–296
- `lib/src/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.cpp`, line 191 (`Cplex::get_var_ray`)

**What happens:**

`save_ray` (Model.h:376) throws unconditionally when the model status is not
`Unbounded`:
```cpp
// Model.h:369-392
static auto save_ray(const Model &t_original_model, const Model &t_model) {
    if (t_model.get_status() != Unbounded) {
        throw Exception("Ray not available.");   // ← line 376
    }
    for (const auto &var: t_original_model.vars()) {
        const double value = t_model.get_var_ray(var);  // → Cplex::get_var_ray
        ...
    }
}
```

`Cplex::get_var_ray` (Optimizers_Cplex.cpp:191) throws the same message when
`m_primal_ray` was never populated:
```cpp
double Cplex::get_var_ray(const Var& v) const {
    if (!m_primal_ray) throw idol::Exception("Ray not available.");
    ...
}
```

`m_primal_ray` is populated in `Cplex::save_infeasibility_information`
(Optimizers_Cplex.cpp:183) only when `s == Unbounded && !m_is_mip`.
After an internal re-solve with primal simplex, the CPLEX status can shift; if
it does, `m_primal_ray` remains empty and the subsequent call via `save_ray`
throws.

This is triggered at two call sites in `ColumnGeneration`:
```cpp
// ColumnGeneration.cpp:131-132  (master ray after re-solve)
if (m_status == Unbounded) {
    m_master_primal_ray = save_ray(master);   // m_status is pre-re-solve value
}

// ColumnGeneration.cpp:294-296  (sub-problem ray)
if (model.get_status() == Unbounded) {
    auto ray = save_ray(model);
}
```

At line 131, `m_status` is assigned from `master.get_status()` **before** the
re-solve at lines 92–96. After the re-solve, `master.get_status()` can differ.
If `m_status == Unbounded` (pre-re-solve) but `master.get_status() != Unbounded`
(post-re-solve), `save_ray` throws.

**Affected instances — all crash in < 1 s:**
```
job-shop-scheduling_abz5    ft06    la25    la27    la35    orb08
nrp_sprint06                nrp_sprint_late03_p
train-timetabling_cycle-T15S8L8Z1W1P3
```

**Suggested fix:**

Re-read `master.get_status()` after the internal re-solve before calling
`save_ray`:
```cpp
// ColumnGeneration.cpp, after lines 92-96:
const auto status_post_resolve = master.get_status();
m_status = status_post_resolve;   // override stale pre-re-solve value
...
if (m_status == Unbounded) {
    m_master_primal_ray = save_ray(master);
}
```

Also guard `Cplex::save_infeasibility_information` against `CPXgetray` failure:
```cpp
// Optimizers_Cplex.cpp:183
int rc = l.CPXgetray(m_env, m_model, m_primal_ray->data());
if (rc != 0) {
    m_primal_ray.reset();   // ray unavailable — do not mark valid
}
```

---

## HF2 — Column generation does not converge at root node (20 instances)

**Files:**
- `lib/src/mixed-integer/optimizers/dantzig-wolfe/ColumnGeneration.cpp`,
  `check_stopping_criterion()` (line 246)

**What happens:**

On 20 instances from the `nrp`, `tup`, `pcp`, and `scheduling` families, the
B&P solver explores exactly **1 B&B node** in 3600 s. The root-node column
generation loop iterates for the entire time budget without converging:

```cpp
// ColumnGeneration.cpp:258-260
if (m_parent.get_remaining_time() <= 0) {   // only exit after full time budget
    m_reason = TimeLimit;
    return true;
}
```

The time check fires only between pricing iterations. With a default iteration
limit of `Inf`, the loop runs until the clock expires.

These are not hard instances — Coluna (B&C&P) solves all 20 in 37–84 s:
```
nrp_long_hint02           idol: 3600 s, 1 node    coluna: 43 s ✓
tup_umps14A_7_3           idol: 3600 s, 1 node    coluna: 37 s ✓
pcp_n120p5t2s2            idol: 3600 s, 1 node    coluna: 53 s ✓
scheduling_U_3_0050_05_1  idol: 3600 s, 1 node    coluna: 45 s ✓
(20 instances total)
```

The **recommended config** (Wentges ρ=0.4) also produces 1 node on the same
instances — Wentges alone is insufficient.

**Suggested investigation:**

1. Enable dual-bound logging at the root for `nrp_long_hint02` to distinguish
   oscillation from slow monotone convergence.

2. Try stronger root-node stabilisation:
   - Boxstep / projected subgradient
   - Dual price smoothing with larger ρ
   - In-out stabilisation

3. Consider a root-node CG iteration cap (e.g., 10 000) that triggers
   branching on the best available bound when LP optimality is not reached
   within budget.

4. Verify that `with_nested_resource_limits` correctly propagates a per-
   iteration time budget to pricing sub-problems, so a single slow pricing call
   cannot consume the full remaining time.

---

## HF3 — Empty block in DEC not handled (1 instance)

**Files:**
- DEC parser / DW formulation builder (search codebase for `"Empty block"`)

**What happens:**

```
vcol_gcol30.txt: exception: Empty block 178 cannot be represented by
                 idol's current Dantzig-Wolfe formulation.
```

The DEC has 2 809 569 blocks; block 178 contains no variables or constraints.
GCG writes empty blocks in some decompositions (the block contributes only
through the implicit setup variable / convexity constraint).

**Suggested fix:**

Skip empty blocks with a one-time warning rather than throwing.  An empty
subproblem is valid: its only content is `PricingSetupVar` (lb=1, ub=1).

---

## Reproduction environment

```
HPC:      isdm (LIRMM cluster, cpu-dedicated partition)
CPLEX:    22.2.0  (libcplex2220.so)
idol:     codex branch, SHA 56e013dc6f5d84450ae31e08357887a63909a204
Configs:  bare (MostInfeasible/BestBound, no stabilisation, 1 thread, seed 0)
          recommended (Wentges ρ=0.4, same base)
Limits:   3600 s, 10 GB, 1 thread
```

Instance files:
```
isdm:/home/lefebvreh/Research/solvers/idol-benchmark/instances/milp/striplib/
```

Minimal reproduction — HF1 (Ray crash):
```bash
bin/idol_bap_runner_56e013dc6f5d \
  instances/milp/striplib/striplib_job-shop-scheduling_abz5.lp \
  instances/milp/striplib/striplib_job-shop-scheduling_abz5.dec \
  --config bare --time-limit 60
# Observed: exit 1, notes="exception: Ray not available."
```

Minimal reproduction — HF2 (pricing hang):
```bash
bin/idol_bap_runner_56e013dc6f5d \
  instances/milp/striplib/striplib_nrp_long_hint02.lp \
  instances/milp/striplib/striplib_nrp_long_hint02.dec \
  --config bare --time-limit 300
# Observed: 1 node explored in 300 s, no solution
```
