# Installing the idol agent kit

This package is designed for the root of `hlefebvr/idol`. It contains instructions and skills only; it does not modify C++ code, CI, or the external HPC repository. Reviewed against the main-branch structure available on 25 November 2026 (tree SHA `8f773675cbd7e56ffd96661231eb0a3b88155c49`). It is not an exhaustive source audit or proof of existing implementations.

## Install

From an existing local checkout, review any existing instruction files first. Extract the ZIP at the repository root, preserving hidden directories:

```sh
cd /path/to/idol
git switch -c agent-guidelines
unzip -n /path/to/idol-agent-kit.zip
python3 scripts/check_agent_skills.py
git status --short
```

`-n` prevents overwriting existing files; merge any conflicting instructions deliberately. Review all additions before committing:

```sh
git add AGENTS.md CLAUDE.md SKILLS.md README-AGENTS.md .agents/skills .claude/skills scripts/check_agent_skills.py
git diff --cached --stat
git diff --cached
git commit -m "Add shared coding-agent guidelines and skills"
```

No global installation is necessary. Start Codex or Claude Code from the repository root (or select the repo in the relevant agent environment). Codex reads `AGENTS.md` and discovers project skills under `.agents/skills/`; Claude Code reads `CLAUDE.md`, which imports `AGENTS.md`, and discovers `.claude/skills/`. `SKILLS.md` is a human/agent index, not a native automatic skill manifest. See [Codex docs](https://developers.openai.com/codex/) and [Claude Code skills](https://code.claude.com/docs/en/skills) for environment-specific details.

## First session

Ask each agent: "Read the repository instructions and skill index. Explain idol's optimizer lifecycle, composition and validation policy. Identify the skills you would use to implement a new optimizer. Do not modify files." Check that it understands lazy reconstruction, mathematical validity, the coding style and HPC validation boundaries.

A typical subsequent task: "Implement [algorithm] from [reference], using the implement-optimizer skill. Preserve existing APIs. Start by describing assumptions, optimizer composition and the validation plan; then implement and test. Report any limitations and prepare an HPC handoff."

Use separate branches or worktrees if Codex and Claude are working concurrently. Do not let two agents edit the same working tree without coordination. Review diffs and run relevant local/integration checks before merging; external validation is a separate gate. No agent should mark a revision HPC validated without reviewed campaign evidence.

## Maintain one source of truth

- `AGENTS.md`: mandatory principles shared by both agents.
- `CLAUDE.md`: thin Claude adapter; do not duplicate policies here.
- `SKILLS.md`: index and repository map.
- `.agents/skills/`: canonical detailed workflows; `.claude/skills/`: identical portable copies (rather than symlinks, to avoid checkout/platform differences).
- `scripts/check_agent_skills.py`: detects missing, extra or divergent copies. After editing canonical skills, run `python3 scripts/check_agent_skills.py --sync` to regenerate Claude copies, then review the diff and commit both. Sync refuses to remove extra Claude skills; remove obsolete managed copies deliberately.

The kit does not enforce policy at runtime. Agent compliance must be checked through code review, tests and independent HPC results. Actual automatic loading may depend on the installed agent version and configuration; the first-session check above is recommended.
