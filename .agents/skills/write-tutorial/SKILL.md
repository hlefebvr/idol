---
name: write-tutorial
description: Write or complete mathematical optimization tutorials for idol's C++ library and CLI, with concise mathematics, verified examples, references, and Doxygen conventions.
---

# Write an idol tutorial

Follow AGENTS.md and TODO-WEBSITE.txt. Use document-feature for related API
reference work. Work from the current target branch; never assume main or
historical examples describe the current API.

## Audience and style

Write for researchers and PhD students familiar with MILPs, elementary LP
duality and (for C++ tutorials) basic C++. Do not assume specialist knowledge
of decomposition, robust or bilevel methods. Explain enough mathematics to
make the code intelligible; do not write a lecture or reproduce a textbook.
Use concise, precise prose, explicit names, small code blocks and rare code
comments (only where the code cannot explain the mathematical decision).
Keep a consistent running example and notation. Avoid promotional filler.

## Procedure

1. Inspect the existing page, nearby tutorials, examples, public headers,
   relevant implementations, documentation build workflow and mathematical
   manual. Identify missing content, actual supported features and prerequisites.
   Preserve existing useful material, page IDs and navigation. For substantial
   algorithm tutorials, agree on the outline with the maintainer first.
2. Define the learning objective and problem class. Present a compact
   mathematical formulation, define symbols, state assumptions and objective
   sense. Explain the algorithmic principle and why each component is needed.
   Distinguish primal solutions, valid bounds, certificates, stopping reasons,
   exact algorithms and heuristics. Verify every numerical example independently.
3. Cite original papers or authoritative references when appropriate, using
   the existing documentation citation conventions. Verify author, title, year,
   DOI/link and relevance. Do not invent references or present an implementation
   as identical to a paper without checking. Put references in context; do not
   create a central bibliography page.
4. Provide a minimal complete working example first, then explain focused
   snippets in the order the reader encounters them. For C++ use idiomatic idol
   objects and factories, correct headers, meaningful identifiers, explicit
   suboptimizer choices and status checks before accessing solution values.
   Keep examples solver-independent where possible, while identifying the
   concrete solver and capabilities required for reproducibility. Provide the
   complete source file and input data via existing examples conventions.
   Register it with the existing example build system where appropriate.
5. For CLI tutorials, use real commands checked against the current parser and
   help. Supply small input files, required flags, exact invocation and expected
   output/status. Explain file formats and mathematical meaning of options.
   Avoid hardcoding exact execution times or solver-dependent solution vectors.
6. Add customization sections for available strategies, parameters and their
   effects. Suggest small experiments, with observable outcomes such as bounds,
   nodes or iterations; avoid unsupported performance promises. Explain when a
   feature is conditional or unavailable. Never document a future feature as
   implemented. Respect the Vanderbeck gate for the branch-and-price tutorial.
7. Use the existing Doxygen Markdown conventions, page IDs, section IDs,
   cross-references, math markup, code fences and navigation. Inspect actual
   configuration and build scripts before running them. Preserve homepage cards
   and visual theme. Avoid duplicate sources of truth and placeholder subpages.
8. Build documentation, check links/math/rendering and inspect the generated
   pages at desktop/mobile sizes when possible. Compile/run complete examples
   with available solvers; check claimed outputs and numerical results. Report
   any missing tooling or licenses and precisely which checks remain undone.
   If a library defect is found, record a minimal reproduction and report it
   separately; do not silently change optimizer code during documentation work.

## Recommended tutorial structure

- Learning goal and prerequisites.
- Running problem and essential mathematics.
- Minimal working solution, with complete source/input.
- Explanation of the algorithm and corresponding API.
- Supported customization and limitations.
- Experiments to try and expected qualitative behavior.
- Relevant scientific references and links to related tutorials.

Adapt the structure to the material; do not force unnecessary sections into
short tutorials. CLI tutorials are task-oriented; C++ algorithm tutorials
emphasize composition and mathematical motivation.

Deliver: updated pages, source/input examples, verified references, a concise
validation summary and unresolved limitations. Do not publish or push without
maintainer approval. Do not perform HPC campaign work.
