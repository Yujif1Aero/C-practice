# Build the crash700 demo (a runnable program that prints HIP error 700).
#   make            -> build ./crash700  (executable)
#   make run        -> build and run it
#   make asm        -> also emit onehop.s / twohop.s (learning assembly)
#   make clean
HIPCC ?= hipcc
ARCH  ?= gfx1100

# default: build the runnable crash700 executable
all: crash700

crash700: crash700.hip.cpp
	$(HIPCC) --offload-arch=$(ARCH) -O3 $< -o $@

run: crash700
	HIP_LAUNCH_BLOCKING=1 ./crash700

# optional: assembly listings for the 1-hop / 2-hop learning demos
asm: onehop.s twohop.s
onehop.s: onehop.hip.cpp
	$(HIPCC) --offload-arch=$(ARCH) --offload-device-only -S -O3 $< -o $@
twohop.s: twohop.hip.cpp
	$(HIPCC) --offload-arch=$(ARCH) --offload-device-only -S -O3 $< -o $@

clean:
	rm -f crash700 onehop.s twohop.s
.PHONY: all run asm clean
