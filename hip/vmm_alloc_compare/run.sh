#!/bin/bash
# Compare ROCm 7.1.1 vs 7.14 VMM behavior, PINNED TO GPU DEVICE 0.
# HIP_VISIBLE_DEVICES=0 hides the second GPU entirely (no wake-ups, no misrouting).
set -u
cd "$(dirname "$0")"
export HIP_VISIBLE_DEVICES=0

echo "########## build ##########"
make VER=711 || exit 1
make VER=714 || exit 1

run_one(){ # $1=ver-tag $2=libdir $3=binary
  local tag=$1 lib=$2; shift 2
  echo
  echo "=========== $* (ROCm $tag runtime, HIP_VISIBLE_DEVICES=0) ==========="
  echo "-- libamdhip64 resolved to:"
  LD_LIBRARY_PATH=$lib ldd "$1" 2>/dev/null | grep amdhip64 | sed 's/^/   /'
  LD_LIBRARY_PATH=$lib "$@"
}

echo
echo "########## vmmgran ##########"
run_one 7.1.1 /opt/rocm-7.1.1/lib       ./vmmgran_711
run_one 7.14  /opt/rocm/core-7.14/lib   ./vmmgran_714

echo
echo "########## vmmcount ##########"
run_one 7.1.1 /opt/rocm-7.1.1/lib       ./vmmcount_711
sleep 8   # let the kernel finish releasing the previous process's allocations
run_one 7.14  /opt/rocm/core-7.14/lib   ./vmmcount_714

echo
echo "########## done ##########"
