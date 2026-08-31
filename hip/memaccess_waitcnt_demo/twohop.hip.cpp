// 2-HOP memory access (pointer chasing): out[i] = *table[idx[i]]
// (1) load a pointer  -> (2) use that loaded value to compute the next address
//     -> load again. The generated asm shows:
//         global_load -> s_waitcnt vmcnt(0) -> v_add_co (address calc) -> global_load
// The order of "s_waitcnt" (wait for data) vs "v_add_co" (use data as address)
// is what MUST be correct. This is the pattern the -g miscompile reorders.
#include <hip/hip_runtime.h>
__global__ void twohop(long** table, int* idx, long* out, int n){
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if(i<n){
    long* p = table[idx[i]];   // (1) read a pointer (an address)
    out[i]  = *p;              // (2) read what that address points to
  }
}
