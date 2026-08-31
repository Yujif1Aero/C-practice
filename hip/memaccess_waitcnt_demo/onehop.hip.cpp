// 1-HOP memory access: out[i] = a[i]
// One load, one wait, one store. The loaded value is NOT used to compute a
// new address, so no address-arithmetic (v_add_co) appears after s_waitcnt.
#include <hip/hip_runtime.h>
__global__ void onehop(long* a, long* out, int n){
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if(i<n) out[i] = a[i];
}
