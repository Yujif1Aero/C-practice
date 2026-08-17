/*
    ======== Flat address offset bug ========

    ==== Steps to reproduce ====

    1. Install ROCm 6.3.1 (I used an Ubuntu 24.04 Docker image, but there's no reason it wouldn't happen elsewhere; I've
       seen thus bug on ArchLinux).
    2. Compile with: /opt/rocm-6.3.1/bin/hipcc --offload-arch=gfx1030 --offload-device-only -O3 -S -o - test.hip


    ==== Actual outcome ====

    This generates the following instructions:
        s_load_dwordx4 s[0:3], s[4:5], 0x0
        s_mov_b64 s[4:5], src_shared_base
        v_lshlrev_b32_e32 v2, 2, v0
        v_mov_b32_e32 v3, 0
        ds_write_b32 v2, v3
        s_waitcnt lgkmcnt(0)
        s_bitcmp1_b32 s3, 0
        s_cselect_b32 s4, s5, s1
        s_cselect_b32 s5, 0, s0
        s_ashr_i32 s3, s2, 31
        s_lshl_b64 s[2:3], s[2:3], 2
        s_sub_u32 s2, s5, s2
        s_subb_u32 s3, s4, s3
        v_mov_b32_e32 v0, s2
        v_mov_b32_e32 v1, s3
        flat_load_dword v0, v[0:1] offset:4
        s_waitcnt vmcnt(0) lgkmcnt(0)
        global_store_dword v3, v0, s[0:1]
        s_endpgm


    ==== Expected outcome ====

    Flat address loads should only use a constant offset when it can be proved that the base address is in the same
    physical memory as the address with the offset applied. I've also attached a test case that can be added to LLVM's
    unit test suite.


    ==== Explanation of problem ====

     - The bug is triggered by the pattern of subtracting an index from a constant. It might be possible to trigger with
       other address calculation patterns.
     - The issue is with: flat_load_b32 v0, v[0:1] offset:4
     - This flat offset load is generated with gfx1100 as well (and probably other ISAs, but those are the ones I
       tested).

    If i is 0 and b is true, then v[0:1] holds an address that's 4 bytes before the start of shared memory. The address
    that would be calculated with the offset:4 is the correct one, but the base address is not recognised by the
    hardware as being for shared memory, and so the GPU crashes.

    There's logic in the compiler to account for the hardware's use of only the base address:
    https://github.com/llvm/llvm-project/blob/ee655ca27aad/llvm/lib/Target/AMDGPU/AMDGPUISelDAGToDAG.cpp#L1756 but it
    only applies when the AMDGPU backend does the optimization. I *think* what's happening with the example in this bug
    report is that the target-independent part of LLVM is being allowed to perform this optimization while only checking
    the result of SIInstrInfo::isLegalFLATOffset and not whether or not such a transformation might lead to an offset
    address that's considered to be in a different physical memory from the base address.

    This bug can be worked around by disabling the application of this optimization to flat address spaces in both
    SIInstrInfo::isLegalFLATOffset and SIInstrInfo::splitFlatOffset. See the attached diff.


    ==== Further commentary ====

    This bug could be fixed in future ISAs by making the hardware choose which memory to fetch from using the offsetted
    address rather than the base address (or adjusting the mapping from address to physical memory so that no valid
    offsetted address has a base address from a different physical memory).

    If that doesn't have any disadvantages, I'd recommend doing that since it'd mean any flat address load/store could
    be optimized with a constant offset, rather than only being able to do so when it'd provably not change the memory
    unit to fetch from. What I don't know is whether this is a hardware bug that's being worked around or a hardware
    property that exists to accelerate some other more important case.

    In either case, there's a bug in compiling for existing ISAs.
*/

#include <hip/hip_runtime.h>
#include <hip/hip_runtime.h>
#include <iostream>
#include <cstdio>

__device__ float explodePlease(float* f, int i)
{
    return f[1 - i]; // This is the address calculation that triggers the bug.
}

__global__ void kernel(float* g, int i, bool b)
{
    __shared__ float s[32];
    s[threadIdx.x] = 0;
    *g             = explodePlease(b ? s : g, i);
}
int main()
{
    int test_mod1 = -1;
    int test_mod2 = 5;
    std::cout << "test remainder :" << test_mod1 << "%" << test_mod2 << "=" << test_mod1 % test_mod2 << std::endl;
    std::cout << "test non-negative modulo :" << test_mod1 << "%" << test_mod2 << "=" << ((test_mod1 % test_mod2) + test_mod2) % test_mod2 << std::endl;
    float* g;
    hipMalloc(&g, sizeof(float) * 64);
    hipMemset(g, 0, sizeof(float) * 64);
    // #4389 の核心: b=true(=shared s), i=1 -> base が shared 領域の手前 -> crash
    kernel<<<1, 32>>>(g, /*i=*/1, /*b=*/true);
    printf("%s\n", hipGetErrorString(hipDeviceSynchronize()));
    return 0;
}

