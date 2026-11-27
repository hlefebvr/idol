\page solver_compatibility Solver Compatibility
\brief Problem classes and algorithmic capabilities supported by each solver through idol.

\tableofcontents

This page describes what idol exposes from each solver — not the solver's native
capabilities. A cell marked ✓ is implemented and exercised by at least one test.
A cell marked ✗ means the wrapper throws an explicit exception. A cell marked
**?** means the wrapper compiles but the behaviour is not covered by an idol test.

For configuration of each solver (environment variables, CMake options, search
paths), see \ref solver_configuration.
For test outcomes per solver, see \ref stable_features.

---

\section compat_problem_classes Problem Classes

<table class="markdownTable">
<thead>
<tr>
  <th>Solver</th>
  <th>LP</th>
  <th>MILP</th>
  <th>QP <span style="font-weight:400;font-size:.85em">(quadratic objective)</span></th>
  <th>QCQP <span style="font-weight:400;font-size:.85em">(quadratic constraints)</span></th>
  <th>SOS1 / SOS2</th>
  <th>License</th>
</tr>
</thead>
<tbody>
<tr><td><b>Gurobi</b></td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✓ / ✓</td><td>Commercial</td></tr>
<tr><td><b>CPLEX</b></td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✓ / ✓</td><td>Commercial</td></tr>
<tr><td><b>HiGHS</b></td><td>✓</td><td>✓</td><td>✗</td><td>✗</td><td>✗ / ✗</td><td>MIT</td></tr>
<tr><td><b>GLPK</b></td><td>✓</td><td>✓</td><td>✗</td><td>✗</td><td>✗ / ✗</td><td>GPL v3</td></tr>
<tr><td><b>Osi</b></td><td>✓</td><td>backend†</td><td>✗</td><td>✗</td><td>backend†</td><td>EPL</td></tr>
<tr><td><b>JuMP</b></td><td>✓</td><td>✓</td><td>?</td><td>✗</td><td>✗ / ✗</td><td>MIT</td></tr>
<tr><td><b>MibS</b></td><td>✗</td><td>✓‡</td><td>✗</td><td>✗</td><td>✗ / ✗</td><td>EPL v2</td></tr>
</tbody>
</table>

† Osi capabilities depend on the backend solver linked at build time (`USE_OSI_CLP`, `USE_OSI_CBC`, `USE_OSI_CPLEX`, …).

‡ MibS solves mixed-integer **bilevel** problems, not general MILPs. It requires the idol customized fork; see \ref solver_config_mibs.

---

\section compat_capabilities Algorithmic Capabilities

Capabilities are reported for the problem class where they apply. Dual-related
information (dual values, reduced costs, Farkas certificates) is only available
for LP or continuous relaxations — not after solving a MIP.

<table class="markdownTable">
<thead>
<tr>
  <th>Capability</th>
  <th>Gurobi</th>
  <th>CPLEX</th>
  <th>HiGHS</th>
  <th>GLPK</th>
  <th>Osi</th>
  <th>JuMP</th>
</tr>
</thead>
<tbody>
<tr><td><b>Dual values</b> (LP)</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td></tr>
<tr><td><b>Reduced costs</b> (LP)</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>?</td></tr>
<tr><td><b>Farkas certificate</b> (infeasible LP)</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>?</td><td>✓</td></tr>
<tr><td><b>Primal ray</b> (unbounded LP)</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>?</td><td>?</td></tr>
<tr><td><b>Callbacks</b></td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✗</td><td>?</td></tr>
<tr><td><b>Lazy constraints</b></td><td>✓</td><td>✓</td><td>✗</td><td>✗</td><td>✗</td><td>?</td></tr>
<tr><td><b>MIP solution pool</b></td><td>✓</td><td>✓</td><td>1 only</td><td>1 only</td><td>n/a</td><td>?</td></tr>
<tr><td><b>Solver-specific parameters</b></td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>?</td><td>?</td></tr>
<tr><td><b>Re-optimize after edits</b></td><td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>?</td><td>?</td></tr>
</tbody>
</table>

**Lazy constraints** are added during the solve via a callback (`with_lazy_cut(true)` on the factory). Only Gurobi and CPLEX expose this through idol.

**MIP solution pool**: Gurobi and CPLEX retain multiple solutions found during branch-and-bound, accessible through `Model::get_n_solutions` and `Model::set_solution_index`. HiGHS and GLPK report at most one solution.

**Re-optimize after edits**: changes to bounds, objective, right-hand sides and the constraint matrix are propagated incrementally to the solver without a full rebuild.

\note Results marked **?** are not covered by idol's current test suite. They may work correctly but have not been verified through idol. Contributions and test reports are welcome on the <a href="https://github.com/hlefebvr/idol/issues" target="_blank">GitHub issue tracker</a>.
