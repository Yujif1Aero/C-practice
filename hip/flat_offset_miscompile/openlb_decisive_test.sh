#!/usr/bin/env bash
# ===========================================================================
# DECISIVE TEST: does OpenLB sphere3d Branching still crash at -O3 on ROCm 7.1,
# and if so, capture the actual faulting `flat_load ... offset:` instruction.
#
# Phases:
#   1. Branching + -O3  -> run  (does it crash on 7.1 at all?)
#   2. Branching + -O0  -> run  (A/B: does -O0 fix it, as before?)
#   3. Disassemble both device binaries, grep flat_load-with-offset in the
#      refinement coupling kernel, and DIFF -O3 vs -O0.
#   4. Restore original config (FEATURES := GPU_CYCLIC_VMEM, HIP_CXXFLAGS := -O3).
#
# Run from repo root:  bash apps-or-wherever/openlb_decisive_test.sh
# It rebuilds the whole library twice (slow) because FEATURES touches every TU.
# ===========================================================================
set -u
ROOT=/home/ubuntu/work/openLB/olb_devm
EX=$ROOT/examples/gridRefinement/cellCentered/sphere3d
CASE=sphere_setup.xml                      # existing RES mesh in that dir
OUT=$ROOT/apps/yuji/decisive_$(printf %s "test")   # log dir (no Date.now in shell is fine)
OUT=$ROOT/decisive_out
mkdir -p "$OUT"
cd "$ROOT" || exit 2

CONFIG=$ROOT/config.mk
RULES=$ROOT/rules.mk

# ---- backup + restore-on-exit ---------------------------------------------
cp "$CONFIG" "$OUT/config.mk.bak"
cp "$RULES"  "$OUT/rules.mk.bak"
restore(){ echo "== restoring config.mk / rules.mk =="; cp "$OUT/config.mk.bak" "$CONFIG"; cp "$OUT/rules.mk.bak" "$RULES"; }
trap restore EXIT

set_branching(){ sed -i 's/^FEATURES *:=.*/FEATURES        :=/' "$CONFIG"; }   # empty => Branching (HIP default)
set_opt(){ sed -i "s/^\(\s*\)HIP_CXXFLAGS := -O[0-9] -std=c++20/\1HIP_CXXFLAGS := -$1 -std=c++20/" "$RULES"; }

show(){ echo "  FEATURES: $(grep -E '^FEATURES' "$CONFIG")"; echo "  HIP_CXXFLAGS: $(grep -nE 'HIP_CXXFLAGS := -O' "$RULES" | head -1)"; }

build_run(){ # $1 = tag (O3/O0)
  local tag=$1
  echo "############################################################"
  echo "# PHASE build_run  tag=$tag"
  set_branching; set_opt "$tag"; show
  echo "== make clean (root) =="; make clean >/dev/null 2>&1
  echo "== make (this takes a while) =="
  if ! make -j"$(nproc)" > "$OUT/build_$tag.log" 2>&1; then
     echo "  BUILD FAILED tag=$tag  (see $OUT/build_$tag.log)"; tail -20 "$OUT/build_$tag.log"; return 1; fi
  echo "  build OK"
  cp "$EX/sphere3d.o" "$OUT/sphere3d_$tag.o" 2>/dev/null
  cp "$EX/sphere3d"   "$OUT/sphere3d_$tag"   2>/dev/null
  echo "== run sphere3d ($tag) =="
  ( cd "$EX" && HIP_VISIBLE_DEVICES=0 timeout 300 ./sphere3d --case-file "$CASE" ) \
      > "$OUT/run_$tag.log" 2>&1
  echo "  exit=$?  (see $OUT/run_$tag.log)"
  if grep -q "HIP error 700" "$OUT/run_$tag.log"; then echo "  >>> HIP error 700 (crash) at $tag"; else echo "  >>> no 700; last lines:"; tail -3 "$OUT/run_$tag.log"; fi
}

disasm(){ # $1 = tag ; extract gfx1100 code object, disasm, grep flat_load offset in coupling
  local tag=$1 obj="$OUT/sphere3d_$tag.o"
  echo "############################################################"
  echo "# PHASE disasm  tag=$tag"
  [ -f "$obj" ] || { echo "  no $obj"; return; }
  roc-obj-ls "$obj" > "$OUT/rocls_$tag.txt" 2>&1
  # extract all code objects next to it
  ( cd "$OUT" && roc-obj-extract "sphere3d_$tag.o" >/dev/null 2>&1 )
  local co
  co=$(ls "$OUT"/sphere3d_$tag.o-*gfx1100* 2>/dev/null | head -1)
  [ -z "$co" ] && co=$(ls "$OUT"/*gfx1100* 2>/dev/null | grep "$tag" | head -1)
  if [ -z "$co" ]; then echo "  could not extract gfx1100 code object; roc-obj-ls:"; cat "$OUT/rocls_$tag.txt"; return; fi
  echo "  code object: $co"
  llvm-objdump -d --mcpu=gfx1100 "$co" > "$OUT/disasm_$tag.s" 2>/dev/null
  echo "  flat_load total       : $(grep -c 'flat_load'          "$OUT/disasm_$tag.s")"
  echo "  flat_load with offset : $(grep -c 'flat_load.*offset:' "$OUT/disasm_$tag.s")"
  echo "  global_load total     : $(grep -c 'global_load'        "$OUT/disasm_$tag.s")"
  grep -n 'flat_load.*offset:' "$OUT/disasm_$tag.s" | head -20 > "$OUT/flatoffset_$tag.txt"
  echo "  sample flat_load+offset (first lines) -> $OUT/flatoffset_$tag.txt"
  head -8 "$OUT/flatoffset_$tag.txt"
}

echo "======== ORIGINAL CONFIG ========"; show
build_run O3
build_run O0
disasm O3
disasm O0
echo "############################################################"
echo "# SUMMARY"
echo "  run_O3 700? : $(grep -c 'HIP error 700' "$OUT/run_O3.log" 2>/dev/null)"
echo "  run_O0 700? : $(grep -c 'HIP error 700' "$OUT/run_O0.log" 2>/dev/null)"
echo "  flat+offset O3: $(grep -c 'flat_load.*offset:' "$OUT/disasm_O3.s" 2>/dev/null)"
echo "  flat+offset O0: $(grep -c 'flat_load.*offset:' "$OUT/disasm_O0.s" 2>/dev/null)"
echo "  all artefacts in: $OUT"
echo "  (config.mk/rules.mk will be restored on exit)"
