# Five accessor patterns (A-E) that all compile to global_load / keep the base
# in-bounds, so NONE of them fault. They document WHY a trimmed standalone does
# not reproduce the miscompile (see ../amd_flat_miscompile_repro for one that does).
#
#   make          # build repro_O0 repro_O1 repro_O2 repro_O3
#   make run      # run each optimisation level (all print mismatches=0)
#   make clean

HIPCC := hipcc
ARCH  := gfx1100
OPTS  := O0 O1 O2 O3

all: $(addprefix repro_,$(OPTS))

repro_%: branching_miscompile.hip.cpp
	$(HIPCC) -$* -std=c++17 --offload-arch=$(ARCH) $< -o $@

run: all
	@for o in $(OPTS); do echo "==================== -$$o ===================="; \
	  HIP_VISIBLE_DEVICES=0 ./repro_$$o; done

clean:
	rm -f $(addprefix repro_,$(OPTS))

.PHONY: all run clean
