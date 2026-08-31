# Build with a selectable hipcc:
#   make                 -> ROCm 7.1.1  (suffix _711)
#   make VER=714         -> ROCm 7.14   (suffix _714, uses core-7.14 compiler)
VER      ?= 711
ifeq ($(VER),714)
  HIPCC    := /opt/rocm/core-7.14/bin/hipcc
  ENV      := ROCM_PATH=/opt/rocm/core-7.14
else
  HIPCC    := /opt/rocm-7.1.1/bin/hipcc
  ENV      :=
endif
ARCH     := gfx1100
CXXFLAGS := -O2 --offload-arch=$(ARCH)

all: vmmgran_$(VER) vmmcount_$(VER)

vmmgran_$(VER): vmmgran.hip.cpp
	$(ENV) $(HIPCC) $(CXXFLAGS) $< -o $@

vmmcount_$(VER): vmmcount.hip.cpp
	$(ENV) $(HIPCC) $(CXXFLAGS) $< -o $@

clean:
	rm -f vmmgran_711 vmmcount_711 vmmgran_714 vmmcount_714
.PHONY: all clean
