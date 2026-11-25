# idol — development skill index

`AGENTS.md` contains mandatory policy. Skills are task-specific workflows, not replacements for it. Load only relevant skills; combine them when a change spans responsibilities. The canonical bodies are `.agents/skills/<name>/SKILL.md`; identical copies in `.claude/skills/` support Claude Code. Paths mentioned in skills are relative to the repository root. Inspect actual code and build options before using them.

| Skill | Use when |
|---|---|
| `implement-optimizer` | Creating or changing a complete optimization algorithm or factory |
| `implement-solver-wrapper` | Adding or modifying an external solver adapter |
| `implement-reformulation` | Deriving, transforming or relaxing a mathematical model |
| `implement-strategy` | Adding branching, node selection, pricing, stabilization, callbacks or separation |
| `review-mathematical-correctness` | Auditing assumptions, bounds, statuses, proofs and numerical behavior |
| `test-optimizer-interoperability` | Testing factory reuse, optimizer nesting, backend substitution or model updates |
| `debug-optimizer` | Investigating wrong solutions, bounds, crashes or regressions |
| `document-feature` | Updating public API docs, examples and mathematical documentation |
| `prepare-hpc-validation` | Defining independent scientific validation and external campaign handoff |
| `measure-performance` | Investigating or justifying significant performance optimizations |

For substantial algorithm work use `implement-optimizer`, `review-mathematical-correctness`, `test-optimizer-interoperability`, `document-feature`, and `prepare-hpc-validation` as applicable. Use the other skills for specific subsystems. No skill authorizes otherwise restricted changes.

## Repository map

- `lib/include/idol/general/optimizers/`: base optimizer, factory, algorithm, timer.
- `lib/include/idol/mixed-integer/modeling/`: models, objects, annotations, expressions, variables, constraints, types.
- `lib/include/idol/mixed-integer/optimizers/`: wrappers, branch-and-bound, Dantzig-Wolfe, callbacks, presolve, PADM.
- `lib/include/idol/bilevel/`: descriptions, wrappers, reformulations and algorithms.
- `lib/include/idol/robust/`: descriptions, uncertainty modeling and algorithms including CCG.
- `lib/src/`: corresponding implementations; `lib/CMakeLists.txt`, top-level `CMakeLists.txt`, `cmake/`: build integration.
- `examples/`, `tests/`: examples, local regression and integration checks; `docs/manual/`, `docs/website/`: scientific and API documentation.
- `bin/`: CLI and method managers; `dev/`: development executable, not a replacement for permanent tests.
- `.github/workflows/tests.yml`: current CI; verify actual execution and failures.
