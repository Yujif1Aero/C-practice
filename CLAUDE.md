# C-practice — repo instructions

A personal C/C++ practice repository containing many **independent,
unrelated subprojects** (e.g. `stats-cli/`, `hip/`, `RovertCPP/`,
`std_map_practice/`, `sorting/`, …). This is also where I practice driving
the main loop on Claude Fable 5 as an orchestrator.

## Scope rule (important)
- Work **only within the subdirectory named in the request**. Do not modify,
  refactor, or "clean up" other subprojects.
- When mapping or searching, stay inside the named subdirectory unless I
  explicitly ask for a repo-wide view. The repo is large; a bare "map this
  project" will otherwise scan everything.
- `hip/` contains real GPU miscompile reproductions — treat it as read-only
  reference unless I explicitly ask to change it.

## Build conventions
- Each subproject builds on its own. Most use a local `Makefile` (`make`) or a
  documented `g++` line; toolchain is gcc/g++ 12.3 (C++17/20).
- `stats-cli/`: `make` / `make run`, C++20, warnings shown but OK (build just
  needs to succeed).

## Orchestration
Inherits the global `## Orchestration` policy (`~/.claude/agents/`:
`scout`, `implementer`, `researcher`, `verifier`; delegate substantial or
parallel work, keep trivial work inline, pick the model tier by stakes).

Project-level tuning:
- **`verifier` → C/C++-tuned, `opus`** (override in `.claude/agents/`). C/C++
  correctness (UB, memory, edge cases) is the highest-stakes thing here, so the
  correctness gate gets a strong model and C++-specific checks. Change its one
  `model:` line to `claude-fable-5` for maximum rigor.
- `implementer` (sonnet) and `scout` (haiku) stay at the global defaults.
- `researcher` is rarely needed for self-contained exercises; only reach for it
  when a task genuinely needs the web.

## Fable practice
`stats-cli/` is the sandbox for practicing the delegation workflow — see
`stats-cli/README.md` for graded practice tasks. Switch the main loop with
`/model claude-fable-5`, then give a goal scoped to `stats-cli/` and let it
delegate.
