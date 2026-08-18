# stats-cli — project instructions

A tiny C++20 practice project (summary statistics from stdin). Used to
practice driving the main loop on Claude Fable 5 as an orchestrator.

## Build / run
- Build: `make`
- Run:   `make run`  (or `echo "1 2 3" | ./stats`)
- Standard: C++20. Warnings are shown (`-Wall -Wextra`) but are OK — don't
  block on them; the build just needs to succeed.

## Orchestration for this project

Inherits the global `## Orchestration` policy (`~/.claude/agents/`,
delegate-when-substantial, pick tier by stakes). Project-specific tuning below.

### Model tier decisions (the point of this exercise)

Relative to the global defaults, for this project:

- **`verifier` → tuned for C++, kept at `opus`** (project override in
  `.claude/agents/verifier.md`). C++ correctness is the highest-stakes thing
  here — undefined behavior, off-by-one, empty-input and even/odd-median edge
  cases, integer/size_t pitfalls — so the correctness gate gets a strong model
  and C++-specific instructions. To push rigor to the maximum, change that
  file's one `model:` line to `claude-fable-5`; opus is the cost-reasonable
  default.
- **`implementer` → stays `sonnet`.** Writing/editing this small, well-scoped
  C++ is medium-stakes; sonnet is the right cost/quality point.
- **`scout` → stays `haiku`.** The codebase is tiny; reading it is low-stakes.
- **`researcher` → not overridden (stays `sonnet`), and rarely needed here.**
  This exercise is self-contained C++ with no external research to do. Only
  reach for it if a task genuinely needs the web (e.g. "what does the C++
  standard say about X") — and only then consider bumping it to `opus`.

Rule of thumb this encodes: **bump the model only where a mistake is costly
and hard to catch.** For a self-contained practice project that's the
verifier, not everything.

### Workflow to practice
1. Switch the main loop to Fable: `/model claude-fable-5`.
2. Give Fable a goal (see README). Let it plan and delegate:
   `scout` to map, `implementer` to make the change, `verifier` to check.
3. Keep trivial edits inline; delegate only substantial/parallel work.
