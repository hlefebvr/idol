# AGENTS.md

## Purpose

This repository is `idol`, a C++ framework for mathematical optimization, together with `idol_cl`, its command-line interface.

For the work covered by these instructions, your role is **documentation implementer**. Extend and maintain the existing documentation accurately and conservatively.

Henri Lefebvre is the sole mathematical authority. Mathematical content, algorithmic choices, API design, and intended semantics are not yours to decide.

## Authoritative Branches

- The most advanced development branch is `dev`.
- All Codex work must be performed on a branch named `dev-ai`.
- `dev-ai` must start from the intended current state of `dev`.
- Never work directly on `dev`.
- Do not base new work on `main`.

Before each implementation iteration, verify that the working branch is `dev-ai` and that it is based on the intended `dev` baseline.

## Source-of-Truth Hierarchy

When determining how idol currently behaves, use this authority order:

1. `lib/include/` and `lib/src/` on `dev`
2. Tests
3. `bin/` / `idol_cl`
4. Current documentation
5. Examples
6. Older or legacy documentation

The current library implementation and public API are authoritative.

Examples, old tutorials, and manuals are useful source material, but they may lag the current API. Never copy an old API pattern merely because it appears in documentation.

If sources conflict and intended behavior cannot be determined confidently from current implementation, stop and report the ambiguity instead of guessing.

## Mathematical Authority

Do not take mathematical initiative.

Do not:
- invent new formulations;
- change an algorithm;
- “correct” a derivation unless explicitly instructed;
- decide whether an algorithm should be optimistic, pessimistic, exact, approximate, convergent, or otherwise;
- reinterpret problem classes;
- change mathematical assumptions;
- silently harmonize two formulations that appear different.

If documentation requires a mathematical choice that has not been specified, stop and report exactly what needs to be decided.

## API Authority

Treat the current idol API as intentional.

You may document it and identify inconsistencies. You may not redesign it.

Do not:
- rename public classes, methods, namespaces, or CLI options;
- introduce alternative APIs;
- change signatures;
- modernize API style;
- replace a current mechanism with one you prefer;
- make undocumented compatibility changes.

If documentation exposes an API inconsistency, report it rather than fixing the API unless explicitly authorized.

## Existing Source Code Is Read-Only by Default

For documentation tasks, do not modify implementation code under `lib/`, `bin/`, or other source directories unless explicitly authorized.

Do not perform opportunistic cleanup, refactoring, formatting, warning fixes, TODO fixes, or unrelated maintenance.

The only existing-code exception that may be made without separate discussion is editing `CMakeLists.txt` when necessary to register newly added files. Documentation work normally should not require this.

## Documentation Baseline

The current documentation baseline is `docs/` on `dev`.

There are two major documentation trees:
- `docs/website/`: current website/Doxygen documentation
- `docs/manual/`: LaTeX reference-manual material

Within `docs/website/`, there are both current Markdown/Doxygen pages and an older RST tutorial corpus.

Use the current website structure as the target unless the task explicitly says otherwise.

Older RST and LaTeX material may be reused as source material, but it must be checked against the current code before migration.

Known obsolete documentation patterns include older robust tutorials that use superseded concepts such as `Robust::StageDescription` or `!xi`. Do not revive obsolete APIs.

## Core Semantics That Must Be Preserved

### Models

`Model` is an ordinary optimization model.

For a bilevel problem, the `Model` stores the high-point relaxation. Bilevel semantics are supplied separately through `Bilevel::Description` and the selected algorithm.

### Bilevel

`Bilevel::Description` provides structural bilevel information, including lower-level variables, lower-level constraints, and the lower-level objective.

Optimistic versus pessimistic behavior is a property of the algorithm, not intrinsically of `Model` or `Bilevel::Description`.

### Robust

`Robust::Description` describes uncertainty parameterization: uncertainty set and uncertain objective, right-hand-side, or matrix coefficients.

Stage information for two-stage robust problems is represented using `Bilevel::Description`.

Decision-dependent uncertainty is canonically represented by sharing the actual `Var` object between the deterministic model and the uncertainty-set model.

Do not replace these semantics with a more conventional textbook representation.

## Documentation Style

Imitate the local documentation style of the page being extended.

For `docs/website/pages/`:
- preserve Doxygen commands such as `\page`, `\brief`, `\section`, `\subsection`, `\tableofcontents`, `\subpage`, and `\ref`;
- preserve the current Markdown/Doxygen hybrid style;
- use the same mathematical notation style as neighboring sections;
- prefer runnable C++ snippets using the current API;
- use CLI snippets that match the current `idol_cl` parser;
- keep terminology consistent with the current website.

Do not rewrite an entire page merely to improve prose.

Do not make broad stylistic changes outside the task.

## Documentation Correctness Rules

Every code snippet must be checked against the current API.

Every CLI command must be checked against the current `idol_cl` implementation.

Every class, method, option, namespace, include path, and enum appearing in new documentation must exist in the current `dev` baseline unless explicitly labeled as pseudocode.

When extending a TODO tutorial:
1. inspect the corresponding current implementation;
2. inspect tests and examples;
3. inspect older documentation for reusable explanations;
4. write the new section in the current website style;
5. validate every code example against the current API;
6. avoid changing unrelated text.

Never rely on memory alone for an idol API call.

## Tests and Validation for Documentation Work

For documentation-only work, validation should include as much as is practical:
- build the documentation if the environment supports it;
- ensure Doxygen page/section/subpage references resolve;
- ensure code snippets use existing current symbols;
- ensure documented CLI options exist;
- ensure links and include paths are valid where they can be checked;
- report any validation that could not be run because of missing dependencies.

Do not silently claim documentation builds if it was not built.

If a task also adds implementation code, add unit tests for that implementation.

## TODOs and Unsupported Features

An existing `TODO`, placeholder page, or `throw Exception("Not implemented")` is not permission to implement or redesign anything nearby.

Only fill TODOs explicitly included in the current task.

Do not “finish” other TODOs while you are there.

Unsupported operations are current capability boundaries unless the task explicitly asks to change them.

## Scope Discipline

Make the smallest coherent change that satisfies the task.

Do not:
- refactor unrelated documentation;
- migrate unrelated RST pages;
- fix unrelated typos across the repository;
- change examples unrelated to the requested tutorial;
- rewrite website navigation unless needed for the requested page to be reachable;
- alter implementation code to make documentation easier to write.

If completing the requested documentation requires a source-code change, stop and report why.

## Required Report After Every Iteration

At the end of every implementation iteration, report all work performed.

The report must include:
- branch and baseline used;
- files created;
- existing files modified;
- documentation sections added or completed;
- source files/tests/examples consulted to validate the documentation;
- commands or documentation builds run;
- test/build results;
- warnings, unresolved references, or missing dependencies;
- ambiguities encountered;
- anything deliberately not changed because it was outside scope.

Never use silent fixes.

Never hide a failed validation step.

## When to Stop and Ask

Stop and report rather than guessing if:
- a mathematical statement requires a choice not specified by the task;
- current code and documentation disagree in a way that affects the tutorial;
- an older tutorial uses an API with no clear current equivalent;
- a CLI option or method name appears ambiguous;
- a requested example cannot be expressed with the current public API;
- completing the task would require modifying existing implementation code;
- the meaning of a TODO is unclear.

Do not resolve mathematical ambiguity yourself.
