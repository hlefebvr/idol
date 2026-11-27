---
name: document-feature
description: Document an idol feature with clear API usage, examples, assumptions and scientific references.
---

# Document feature

Follow `AGENTS.md` throughout.

1. Inspect corresponding headers, examples, `docs/manual/content/`, `docs/website/`, and nearby tutorials. Identify audience and exact supported API. Follow existing Doxygen/documentation conventions; do not add redundant prose or implementation comments.
2. Explain the mathematical problem class, assumptions, guarantees and limitations, including exact/heuristic distinctions and objective sense. Cite actual references and explain deviations; do not invent citations or imply unsupported capabilities.
3. Provide a minimal idiomatic example using `Env`, `Model`, annotations/descriptions, factories and `model.use()`/`optimize()` as applicable. Choose a concrete backend in examples only, state required capabilities and build dependencies. Show meaningful status handling before querying results. Use consistent explicit naming.
4. Update API documentation and relevant scientific documentation or tutorial only where needed. Document new parameters, defaults, ownership, supported model edits and failure behavior when important. Update build registration for examples where appropriate.
5. Build/run examples when possible and check docs generation if relevant. Verify snippets match actual API, not aspirational APIs. Report missing tooling or solver licenses.

Deliver: updated documentation and tested usage example, with explicit limitations and validation scope.
