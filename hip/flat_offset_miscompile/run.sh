#!/usr/bin/env bash
# Build the reproducer at -O0 and -O3 and compare.
# Usage: ./run.sh   (edit ARCH for your GPU; gfx1100 = W7900)
set -u
HIPCC=${HIPCC:-hipcc}
ARCH=${ARCH:-gfx1100}
SRC=branching_miscompile.hip.cpp

echo "### compiler: $($HIPCC --version | head -1)"
echo "### arch: $ARCH"
echo

for OPT in O0 O1 O2 O3; do
  echo "==================== -$OPT ===================="
  $HIPCC -$OPT -std=c++17 --offload-arch=$ARCH "$SRC" -o repro_$OPT 2>build_$OPT.log
  if [ $? -ne 0 ]; then echo "  BUILD FAILED (see build_$OPT.log)"; continue; fi
  HIP_VISIBLE_DEVICES=${HIP_VISIBLE_DEVICES:-0} ./repro_$OPT
  echo "  exit=$?"
  echo
done

echo "Interpretation:"
echo "  -O0 correct + -O3 'MISCOMPILE/wrong result' (or HIP error 700)  => amdclang -O3 miscompile"
echo "  all correct                                                     => this trimmed kernel"
echo "     does not trigger it standalone; use the OpenLB sphere3d case as the reproducer."
