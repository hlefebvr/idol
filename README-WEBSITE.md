# idol website work — installation and handoff

This is an additive kit for the existing idol repository, not a replacement
for the original agent kit. It targets the `codex` branch. It contains only
instructions and a new tutorial-writing skill; no website implementation.

1. In `~/Research/Solvers/idol`, verify that the working tree is in a safe state
   and that you are on `codex` (`git status`, `git branch --show-current`).
2. Extract this archive directly into the repository root, including the hidden
   directories. `unzip -n /path/to/idol-website-agent-kit.zip` avoids overwriting
   existing files. If a file already exists, compare and merge it deliberately.
3. In the existing `SKILLS.md` table, add:

   `| write-tutorial | Writing or completing mathematical optimization tutorials for the C++ library or CLI |`

   Keep the existing index, AGENTS.md, CLAUDE.md and TODO.txt intact.
4. Run `python3 scripts/check_agent_skills.py` if the original kit is installed.
   Review `git status --short`, track the new files, and restart the agent.

Suggested agent prompt:

> Work on the codex branch. Read AGENTS.md, SKILLS.md, TODO-WEBSITE.txt and the
> write-tutorial skill. Integrate the skill index entry if missing. Execute the
> website roadmap in order, starting with task 01. Keep the existing homepage
> cards and visual identity. Report actual checks and blockers; do not modify
> library algorithms, run HPC campaigns or push changes. Branch-and-price is
> last and must wait for maintainer confirmation that Vanderbeck branching and
> its public API are ready. Discuss the CCG and B&P tutorial outlines before
> authoring them. Do not change the separate library TODO.txt.

The first explicit AI-related commit observed on dev-ai is 3fc8318ca (August
2026); the repository was created in September 2022. The disclosure uses those
months, based on the maintainer's stated history and agreement, not on an
assumption that Git metadata proves authorship. Recheck if the current history
contradicts these dates.
