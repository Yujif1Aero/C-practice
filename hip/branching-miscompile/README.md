# amdclang `-O3` miscompile of a "pointer-before-array" flat load (OpenLB HIP)

## What this is
A minimal HIP reproducer *sketch* for a bug we hit in **OpenLB** on **AMD (gfx1100 /
W7900, ROCm)**: the grid-refinement coupling does an **illegal memory access
(`HIP error 700`)** at `-O3`, but runs correctly at `-O0`. Everything points to an
**amdclang `-O3` optimizer miscompilation**, not a source bug.

## The pattern (from OpenLB's "Branching" cyclic population store)
LBM streaming is a cyclic (ring) buffer. OpenLB addresses `base[(shift + i) mod N]`
with two pointers and a predicated select instead of a modulo:

```cpp
start0 = base + shift;          // head (valid)
start1 = base - (N - shift);    // tail  <-- POINTER BEFORE THE ARRAY  (UB, [expr.add])
remainder = N - shift - 1;
value = (i > remainder) ? start1[i] : start0[i];   // == base[(shift + i) % N]
```

For every `i` in `[0, N)` the **final** address is in-bounds, but the intermediate
`start1` is a pointer *before* the allocation. On the GPU flat address space,
amdclang `-O3` folds `start1[i]` into a **flat load with a derived byte offset**. If
that folding does not verify the offset stays within the same allocation, the
effective address is wrong → wrong data or illegal access.

## Known precedent (important for the AMD report)
This is the same class as **ROCm issue #4389 — "Flat address space operations can
miscompile"**: e.g. `return f[1 - i];` becomes `flat_load_b32 ... offset:4`, and the
target-independent LLVM optimization only checks `SIInstrInfo::isLegalFLATOffset`, not
whether `base + offset` lands in a different physical allocation. Reported workaround:
disable that folding for flat address spaces in `isLegalFLATOffset` / `splitFlatOffset`.
Our `start1 = base - (N - shift); start1[i]` is exactly a negative-base + positive-index
flat load.

## Why we are confident it's the optimizer (evidence from OpenLB)
Same machine, same `sphere3d` (RES8, 884.7k cells), same **Branching** strategy:

| build | result |
|---|---|
| Branching, **-O3** | `HIP error 700` at step 0 (crash) |
| Branching, **-O0** | runs fine to step 268 (no error) |

And, all consistent with a miscompile:
- **VMM** strategy (`ptr[i]`, no negative pointer) works at -O3.
- **CUDA / nvcc** + Branching works (different compiler).
- Rewriting the accessor UB-free still crashes at -O3 (still a branchy flat load).
- An in-accessor index check shows indices are always in range (source is correct).
- Non-refinement examples (`cylinder3d`) work (only the coupling's code shape miscompiles).
- `rocgdb` hides it (debug build relaxes optimization).

## Experiment result on this machine (ROCm/HIP 7.1, gfx1100) — IMPORTANT
Ran 5 patterns (A: `buf-K`, B: two-pointer select, C: `buf[C-i]`, D: coupling-shaped
Q=19 + runtime shift, E: pointer loaded from a table to force generic/flat) at
`-O0/-O1/-O2/-O3`: **0 mismatches everywhere — no miscompile reproduced standalone.**

Reason, from the device assembly (`-S --offload-device-only`): amdclang emits
**`global_load_*` (addrspace(1)), not `flat_load`**, for all the indexed accesses —
it proves the pointers are global and uses the *simple linear* load, which is not the
#4389-buggy path. Even Pattern E (pointer loaded from memory) only produced one
`flat_load_b64` for the *table read*; the `start1[i]` access folded back to `global_load`.

Takeaways:
1. The #4389 flat-offset fold bug is **only reachable through a genuinely generic
   (`flat`) load with a folded immediate offset**. A trimmed standalone rarely keeps the
   pointer generic — the optimizer recovers global provenance.
2. ROCm 7.1 is newer than #4389's report; the simple cases may be **fixed** in 7.1.
3. => The ground-truth reproducer is still OpenLB sphere3d. To sharpen the AMD report,
   the next step is not another guessed standalone but **extracting the actual faulting
   instruction from OpenLB's compiled refinement TU on 7.1** (`llvm-objdump` the coupling
   `.o`, find the `flat_load ... offset:` on the CyclicColumn path), and confirming
   sphere3d Branching still crashes at `-O3` under 7.1 at all.

## Build & run
```sh
chmod +x run.sh
ARCH=gfx1100 ./run.sh        # builds -O0/-O1/-O2/-O3 and compares to a CPU reference
```
- `-O0 correct` + `-O3 MISCOMPILE/700` → amdclang -O3 miscompile.
- All correct → this trimmed kernel doesn't trigger it standalone (miscompiles are
  context sensitive). Use the **ground-truth reproducer** below.

## Ground-truth reproducer (always reproduces)
```sh
# OpenLB, config: PLATFORMS := CPU_SISD GPU_HIP ; HIP_ARCH := gfx1100 ; FEATURES :=  (Branching)
cd examples/gridRefinement/cellCentered/sphere3d
uv run setup_sphere.py --RESOLUTION 8
make clean && make                        # from repo root
HIP_VISIBLE_DEVICES=0 ./sphere3d --case-file sphere_setup.xml
# -> HIP error 700 at step 0.  Set rules.mk HIP_CXXFLAGS := -O0 and rebuild -> runs.
```

## Workarounds
1. `FEATURES := GPU_CYCLIC_VMEM` — VMM accessor (`ptr[i]`) avoids the miscompiled path. (recommended)
2. Build the refinement TU at `-O0`/`-O1`/`-O2`.
3. Once the offending pass is identified, `-fno-…`-disable it (keeps -O3 elsewhere).

## Files
- `branching_miscompile.hip.cpp` — the standalone sketch.
- `run.sh` — build -O0..-O3 and compare.
