# Forces the buggy INSTRUCTION shape (flat_load ... offset:) but does NOT fault,
# because ingredient (3) is missing: the folded base register stays INSIDE the
# allocation. Companion to ../amd_flat_miscompile_repro (which does fault).
#
#   make disasm   # shows flat_load_b128 ... offset:16/32/...
#   make run      # prints "correct" (no fault) -- expected

HIPCC    := hipcc
ARCH     := gfx1100
HIPFLAGS := -O3 -std=c++17 --offload-arch=$(ARCH)

all: ff

ff: flat_force.hip.cpp
	$(HIPCC) $(HIPFLAGS) $< -o $@

disasm: flat_force.hip.cpp
	$(HIPCC) $(HIPFLAGS) -S --offload-device-only $< -o flat_force.s
	@echo "---- flat_load with immediate offset ----"
	@grep -nE 'flat_load.*offset:' flat_force.s || echo "(none)"

run: ff
	HIP_VISIBLE_DEVICES=0 ./ff              # SAFE mode (positive index) -> "correct"

# FAULT mode: negative-index access buf[1-i]; i>=2 pushes the base out of the
# allocation -> HIP 700. Shows an argument CAN fault it, but only with the
# negative-coefficient access pattern (not the SAFE kernel).
fault: ff
	@for i in 0 1 2 3; do echo -n "i=$$i : "; \
	  HIP_VISIBLE_DEVICES=0 HIP_LAUNCH_BLOCKING=1 ./ff $$i 2>&1 | tail -1; done

clean:
	rm -f ff flat_force.s

.PHONY: all disasm run fault clean
