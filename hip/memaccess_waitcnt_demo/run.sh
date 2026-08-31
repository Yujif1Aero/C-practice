#!/bin/bash
# Build the crash700 program and run it. It deliberately dereferences a bogus
# pointer (0xdeadbeef00), which produces an illegal memory access = HIP error 700.
set -u
cd "$(dirname "$0")"

echo "===== build ====="
make crash700

echo
echo "===== run crash700 (expect: HIP error 700 = illegal memory access) ====="
HIP_LAUNCH_BLOCKING=1 ./crash700
rc=$?
echo "(exit code $rc ; 188 = 700 & 0xFF)"
