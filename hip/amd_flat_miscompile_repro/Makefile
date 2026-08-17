# AMD flat-address "offset folding" miscompile — minimal FAULT reproducer.
# Faithful port of ROCm issue #4389, run (not just -S) on gfx1100 / ROCm 7.x.
#
#   make          # build ./repro4389
#   make disasm   # show the wrong instruction:  flat_load_b32 ... offset:4
#   make run      # run it -> "an illegal memory access" (== HIP error 700)
#   make clean

HIPCC    := hipcc
ARCH     := gfx1100                 # W7900. Check yours with: rocm-smi --showproductname
HIPFLAGS := -O3 -std=c++17 --offload-arch=$(ARCH)

all: repro4389

repro4389: repro_4389.hip.cpp
	$(HIPCC) $(HIPFLAGS) $< -o $@

# Emit device assembly and grep for the miscompiled flat load with an immediate offset.
disasm: repro_4389.hip.cpp
	$(HIPCC) $(HIPFLAGS) -S --offload-device-only $< -o repro_4389.s
	@echo "---- flat_load with immediate offset (the bug) ----"
	@grep -nE 'flat_load.*offset:' repro_4389.s || echo "(none found on this build)"

# Blocking so the async GPU fault surfaces at the exact faulting kernel launch.
run: repro4389
	HIP_VISIBLE_DEVICES=0 HIP_LAUNCH_BLOCKING=1 ./repro4389

clean:
	rm -f repro4389 repro_4389.s

.PHONY: all disasm run clean
