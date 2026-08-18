// Minimal reproduction of ROCm issue #4389 (flat-address offset-folding miscompile).
// Confirmed still present on ROCm 7.1.52802 (clang roc-7.1.1) / gfx1100 (Radeon Pro W7900).
//
// explodePlease() returns f[1-i]. The `b ? s : g` select makes `f` a generic (flat)
// pointer, so the load lowers to a flat_load. At -O3 amdclang folds the access into
//     base = f - i*4 ;  flat_load_b32 v0, v[0:1] offset:4      (effective = f[1-i])
// On gfx11 the hardware decides the memory SPACE from the pre-offset BASE. For the
// faulting case b=1 (f = __shared__ s) and i=1, base = &s[0] - 4 sits 4 bytes BELOW
// the LDS aperture, is misclassified, and the access is illegal (here: a silent GPU
// hang; hipDeviceSynchronize never returns).
//
// IMPORTANT: the source access is fully IN BOUNDS and well-defined for every (b,i)
// swept below (1-i is in [0,3]; s has 32 elements). The out-of-bounds BASE is
// introduced by the compiler's offset folding, NOT by the source -> this is a genuine
// miscompile, not user UB. At -O0 the fold is not performed and no fault occurs.
//
//   Build:  hipcc -O3 -std=c++20 --offload-arch=gfx1100 repro_4389.hip.cpp -o repro4389
//   Run  :  HIP_LAUNCH_BLOCKING=1 ./repro4389        # hangs at b=1 i=1 on a fresh GPU
//   ASM  :  hipcc -O3 --offload-arch=gfx1100 -S --offload-device-only repro_4389.hip.cpp -o - \
//             | grep 'flat_load.*offset:'            # shows the miscompiled instruction
#include <hip/hip_runtime.h>
#include <cstdio>

__device__ float explodePlease(float* f, int i) { return f[1 - i]; }

__global__ void kernel(float* g, int i, bool b)
{
    __shared__ float s[32];
    s[threadIdx.x] = 0;
    __syncthreads();
    *g = explodePlease(b ? s : g, i);
}

int main()
{
    float* g;
    if (hipMalloc(&g, sizeof(float) * 64) != hipSuccess) {
        printf("malloc fail\n");
        return 2;
    }
    (void)hipMemset(g, 0, sizeof(float) * 64);
    // sweep i and b; blocking so a fault surfaces at the faulting launch
    for (int b = 0; b < 2; ++b) {
        for (int i = -2; i < 2; ++i) {
            kernel<<<1, 32>>>(g, i, (bool)b);
            hipError_t le = hipGetLastError();
            hipError_t se = hipDeviceSynchronize();
            printf("b=%d i=%2d : launch=%s sync=%s%s\n", b, i, hipGetErrorString(le), hipGetErrorString(se),
                   se != hipSuccess ? "   <<< FAULT (miscompile)" : "");
            if (se != hipSuccess) {
                printf(">>> reproduced #4389-style fault at b=%d i=%d\n", b, i);
                return 1;
            }
        }
    }
    // Every (b,i) above is in-bounds/well-defined; reaching here means the compiler's
    // out-of-bounds base did not fault this run (the miscompiled instruction is still
    // emitted -- see the -S disassembly -- but the illegal base was tolerated).
    printf("no fault on this run (miscompiled instruction still present; latent)\n");
    return 0;
}
