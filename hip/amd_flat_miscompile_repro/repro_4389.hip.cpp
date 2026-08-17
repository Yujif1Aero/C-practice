// Faithful reproduction of ROCm issue #4389 (flat address space miscompile),
// run (not just -S) on this GPU. The `b ? s : g` select forces a generic/flat
// pointer; `f[1-i]` folds to `flat_load ... offset:4`; the HW uses the BASE
// (pre-offset) address to decide the memory SPACE, so when base sits just before
// the __shared__ aperture it is misclassified -> illegal access.
#include <hip/hip_runtime.h>
#include <cstdio>

__device__ float explodePlease(float *f, int i){ return f[1 - i]; }

__global__ void kernel(float *g, int i, bool b){
    __shared__ float s[32];
    s[threadIdx.x] = 0;
    __syncthreads();
    *g = explodePlease(b ? s : g, i);
}

int main(){
  float* g; if(hipMalloc(&g,sizeof(float)*64)!=hipSuccess){printf("malloc fail\n");return 2;}
  hipMemset(g,0,sizeof(float)*64);
  // sweep i and b; blocking so a fault surfaces at the faulting launch
  for(int b=0;b<2;++b){
    for(int i=-2;i<=4;++i){
      kernel<<<1,32>>>(g,i,(bool)b);
      hipError_t le=hipGetLastError();
      hipError_t se=hipDeviceSynchronize();
      printf("b=%d i=%2d : launch=%s sync=%s%s\n", b, i,
             hipGetErrorString(le), hipGetErrorString(se),
             se!=hipSuccess? "   <<< FAULT (miscompile)":"");
      if(se!=hipSuccess){ printf(">>> reproduced #4389-style fault at b=%d i=%d\n",b,i); return 1; }
    }
  }
  printf("no fault on this build\n");
  return 0;
}
