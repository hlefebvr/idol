# SKILLS.md

# Skill: Writing and Extending idol Documentation

## Mission

Extend the existing idol documentation accurately, using the current `dev` implementation as the source of truth and the existing `docs/` content as the documentation baseline.

This skill is for documentation tasks involving:
- the `idol` C++ library;
- the `idol_cl` command-line interface;
- mathematical optimization tutorials;
- examples and explanatory derivations;
- completion of TODO-marked documentation sections.

The objective is not to redesign the documentation or the API. The objective is to make the existing documentation more complete while preserving its structure, terminology, and technical meaning.

## 1. Start by Locating the Target Page

Identify the exact source file that generates the requested website page.

For website work, the target normally lives under:

```text
docs/website/pages/
```

Do not edit generated HTML.

For example, a website page named `idol/lib_milp_basics.html` is generated from the corresponding Doxygen page declaration in the Markdown source under `docs/website/pages/lib/...`.

Read the entire target source file before editing it.

Also inspect its immediate parent/index page so navigation and naming conventions are understood.

## 2. Enumerate the Explicit TODOs in Scope

Search the target page for:
- `TODO`
- `\warning TODO`
- `(TODO)`

Create a private checklist of only the TODOs covered by the task.

Do not treat every TODO in the repository as in scope.

If the task says “write the missing tutorials marked as TODO in `lib_milp_basics.html`,” the TODO subpages declared from that page are the scope unless the user says otherwise.

Before writing, identify for each TODO:
- intended page identifier;
- intended title;
- neighboring tutorial style;
- current relevant library classes;
- useful old RST/manual material;
- relevant tests/examples.

## 3. Reconstruct Intended Content from Current Sources

For each tutorial, gather evidence in this order.

### A. Current public headers

Read the relevant files in `lib/include/idol/`.

Determine:
- class names;
- constructors;
- factories;
- public methods;
- argument types;
- enums;
- callback interfaces;
- ownership patterns;
- supported operations.

### B. Current implementation

Read the corresponding `lib/src/` implementation.

Use it to understand behavior that the header alone does not make obvious.

Do not infer a new design from implementation quirks.

### C. Tests

Use tests to identify intended public usage and valid combinations.

Tests are particularly valuable for examples that should compile.

### D. Examples

Use examples as secondary evidence.

Check every example pattern against current headers and implementation before copying it.

Examples may be stale.

### E. Existing documentation

Search:

```text
docs/website/
docs/manual/
```

Older RST or LaTeX material may contain a nearly complete explanation of the desired topic.

Reuse its mathematical exposition where appropriate, but translate all API usage to the current API.

## 4. Respect idol’s Concepts

### Environment and object identity

`Env` manages optimization objects and their identities.

Objects such as `Var` and `Ctr` can appear in multiple models with model-specific versions.

Do not document variables or constraints as if they were owned exclusively by one model if the current API semantics are more general.

### Optimizer factories

Distinguish:
- optimizer factories: reusable configuration objects;
- optimizers: model-bound runtime objects.

Document `Model::use(...)` and composition using the terminology already established in neighboring documentation.

### Transformations and wrappers

Some algorithms build transformed models internally.

Some optimizers forward supported parent-model changes.

Others invalidate/rebuild their internal formulation.

Do not promise incremental behavior that the implementation does not provide.

## 5. Writing C++ Tutorial Code

All new code examples should follow nearby idol style.

Prefer examples that:
- create `Env env;`;
- create a `Model`;
- add variables/constraints using the current model API;
- configure an optimizer factory;
- call `model.use(...)`;
- call `model.optimize()`;
- retrieve status/solution only when relevant.

Use current include paths.

Do not introduce helper abstractions merely to make the tutorial shorter.

Do not invent overloads.

Do not show deprecated or old APIs.

If a snippet is intentionally partial, state what surrounding objects are assumed to exist.

## 6. Writing idol_cl Tutorial Code

Before documenting a CLI command:
1. inspect the current parser and method registration under `bin/`;
2. verify the subcommand;
3. verify the option spelling;
4. verify the method name;
5. verify required companion files;
6. verify solver/dependency assumptions.

Do not infer command-line syntax from an old screenshot or sample output.

Use commands in the same form as neighboring CLI tutorials.

## 7. Mathematical Exposition

Mathematical explanations must be faithful to the algorithm already implemented or to mathematical text explicitly supplied by the user.

You may:
- explain notation;
- reorganize an existing derivation for clarity;
- adapt an older valid derivation to the current page;
- connect code objects to mathematical objects.

You may not:
- derive an alternative algorithm;
- change assumptions;
- strengthen claims;
- add a convergence theorem;
- replace the user’s formulation with a conventional one;
- decide that an existing derivation is wrong and silently substitute your own.

If mathematical clarification is needed, report it.

## 8. Completing `lib_milp_basics` TODO Tutorials

When the assigned task is to complete the missing TODO tutorials linked from `lib_milp_basics`, proceed tutorial by tutorial.

The current page identifies advanced topics such as:
- Environment
- Models
- Variables
- Expressions
- Constraints
- Objective function
- Optimizers
- Creating an optimizer

Some are already implemented and some are marked TODO.

For every TODO page:

### Step 1 — Identify the exact current API

Read the corresponding modeling/optimizer classes and implementation.

### Step 2 — Search legacy docs

The older RST tutorials under:

```text
docs/website/tutorials/mixed-integer/modeling/
docs/website/tutorials/mixed-integer/optimizers/
```

contain substantial source material for these concepts.

Useful legacy topics include:

```text
modeling/expressions.rst
modeling/constraints.rst
modeling/objective-function.rst
optimizers/optimizers.rst
optimizers/solving.rst
optimizers/access-optimizer.rst
optimizers/create-universal-callback.rst
optimizers/universal-callbacks.rst
```

Do not paste these mechanically. Reconcile them with `dev`.

### Step 3 — Match the current Markdown tutorial style

Follow the structure used by the already-written `Environment`, `Models`, and `Variables` sections in the current `lib_milp_basics` page:

```text
\page ...
\brief ...
\tableofcontents

\section ...
explanation

```cpp
...
```
```

Prefer progressive examples and short focused sections.

### Step 4 — Keep the page pedagogical

A basics tutorial should explain how a user works with the API, not reproduce the entire Doxygen class reference.

For each concept, prioritize:
- what the object represents;
- how it is created;
- the most important operations;
- interactions with `Model`;
- one or more concrete code examples;
- important constraints or caveats.

Use links/references to class documentation for exhaustive member lists where appropriate.

### Step 5 — Validate

Cross-check every identifier against current headers.

If possible, compile representative snippets or build documentation.

## 9. Page Linking and Doxygen Conventions

Preserve existing page IDs.

When filling an existing TODO subpage, use the page identifier already referenced by the parent page.

Do not create a second competing page ID for the same topic.

Use `\ref`, `\subpage`, and `\copybrief` consistently with surrounding pages.

Avoid raw hyperlinks to another internal page when a Doxygen reference exists.

Ensure section IDs are unique.

## 10. What Counts as a Good Tutorial

A completed tutorial should:
- reflect the current API;
- compile conceptually and preferably literally;
- use idol terminology;
- explain the purpose of the feature;
- show idiomatic usage;
- mention important limitations;
- fit naturally after the neighboring tutorial;
- contain no speculative API;
- contain no unrelated redesign.

A tutorial is not complete merely because the TODO marker was deleted.

## 11. Validation Checklist

Before reporting completion, check:
- [ ] Every requested TODO was addressed.
- [ ] No unrelated TODO was modified.
- [ ] Every documented C++ symbol exists on `dev`.
- [ ] Every documented CLI option exists on `dev`, if applicable.
- [ ] No obsolete API was copied from legacy docs.
- [ ] Doxygen page IDs match parent references.
- [ ] Internal references are valid as far as can be checked.
- [ ] Examples follow current local style.
- [ ] Mathematical statements were not altered beyond the authorized scope.
- [ ] Documentation build was attempted when feasible.
- [ ] All failures or unavailable dependencies are reported.

## 12. Required Final Report

After the documentation iteration, report:

### Files changed
List each file created or modified.

### Tutorials completed
List the TODO tutorials filled in.

### Sources consulted
List the relevant headers, implementations, tests, examples, and legacy RST/manual pages.

### Validation
Report exactly what was run and whether it succeeded.

### Unresolved issues
List ambiguous API behavior, missing dependencies, documentation references that could not be validated, and mathematical questions requiring Henri Lefebvre.

### Scope exclusions
Explicitly mention notable nearby TODOs or stale pages deliberately left unchanged because they were outside the assigned task.

## Golden Rule

**Document what idol currently is. Do not turn documentation work into API design, algorithm design, or repository cleanup.**
