# stats-cli

A tiny C++20 command-line tool that reads whitespace-separated numbers from
stdin and prints count / mean / median / population standard deviation.

```sh
make
echo "1 2 3 4 5 6 7 8 9 10" | ./stats
```

## Purpose

A sandbox for practicing the Fable-orchestrator workflow. Switch the main loop
to Fable (`/model claude-fable-5`), then give it a goal and let it delegate.

## Practice tasks (increasing scope)

1. **Read-only warmup** — Ask: "map this project and tell me what `stats::median`
   does for an even-length input." (Good fit for `scout`.)
2. **Small feature** — "Add a `--sample` flag that switches stddev to the
   sample (n-1) formula." Let Fable delegate the edit to `implementer`, then
   run `verifier` to check the edge cases.
3. **Bug hunt** — Intentionally break something (e.g. change `mid - 1` to
   `mid`), then ask Fable to find and confirm the bug via `verifier`.
4. **Parallel fan-out** — "Add unit tests for mean, median, and stddev" and let
   it spawn work concurrently.

Layout:

```
include/stats.hpp   declarations
src/stats.cpp       implementations
src/main.cpp        CLI entry point
Makefile            make / make run / make clean
CLAUDE.md           project orchestration + model-tier decisions
.claude/agents/     project-level subagent overrides (verifier)
```
