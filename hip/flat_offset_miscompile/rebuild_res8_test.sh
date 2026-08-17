#!/usr/bin/env bash
# CLEAN rebuild of sphere3d in the EXAMPLE dir (so sphere3d.o = the templated
# CyclicColumn/coupling TU is actually recompiled with the current FEATURES),
# Branching + -O3, then run at RES8 with blocking mode. Emits progress lines.
set -u
ROOT=/home/ubuntu/work/openLB/olb_devm
EX=$ROOT/examples/gridRefinement/cellCentered/sphere3d
OUT=$ROOT/decisive_out; mkdir -p "$OUT"
CONFIG=$ROOT/config.mk
cd "$ROOT" || exit 2

cp "$CONFIG" "$OUT/config.mk.bak3"
restore(){ echo "PROGRESS restoring config.mk"; cp "$OUT/config.mk.bak3" "$CONFIG"; }
trap restore EXIT

sed -i 's/^FEATURES *:=.*/FEATURES        :=/' "$CONFIG"   # Branching
echo "PROGRESS FEATURES=$(grep -E '^FEATURES' "$CONFIG")  HIP=$(grep -oE 'HIP_CXXFLAGS := -O[0-9]' "$ROOT/rules.mk" | head -1)"

echo "PROGRESS make clean (root)"
make clean >/dev/null 2>&1
rm -f "$EX/sphere3d" "$EX/sphere3d.o"          # force example TU rebuild
echo "PROGRESS building sphere3d in EXAMPLE dir (this is the real TU)"
if ! ( cd "$EX" && make -j"$(nproc)" ) > "$OUT/build_ex_O3.log" 2>&1; then
  echo "PROGRESS BUILD FAILED"; tail -20 "$OUT/build_ex_O3.log"; exit 1; fi
echo "PROGRESS build OK; sphere3d.o rebuilt? $(ls -la "$EX/sphere3d.o" | awk '{print $6,$7,$8}')"
echo "PROGRESS confirm Branching in binary (grep symbol): $(strings "$EX/sphere3d" | grep -m1 -iE 'Branching|CyclicStrategy' || echo none)"

echo "PROGRESS generating RES8 mesh"
( cd "$EX" && uv run setup_sphere.py --RESOLUTION 8 >/dev/null 2>&1 )
echo "PROGRESS mesh RES=$(grep -oE '<RESOLUTION>[0-9]+' "$EX/sphere_setup.xml" | head -1)"

echo "PROGRESS running (blocking, line-buffered, timeout 300)"
( cd "$EX" && HIP_VISIBLE_DEVICES=0 HIP_LAUNCH_BLOCKING=1 timeout 300 stdbuf -oL -eL \
    ./sphere3d --case-file sphere_setup.xml ) > "$OUT/run_ex_O3_RES8.log" 2>&1
rc=$?
echo "PROGRESS run exit=$rc"
echo "PROGRESS 700count=$(grep -c 'HIP error 700' "$OUT/run_ex_O3_RES8.log")  steps=$(grep -cE 'Timer.*step=' "$OUT/run_ex_O3_RES8.log")"
echo "PROGRESS last lines:"; tail -5 "$OUT/run_ex_O3_RES8.log"
echo "PROGRESS RES8_TEST_DONE"
