// s is now a GLOBAL device array (file scope), NOT __shared__.
// Question: does f[1-i] before s give HIP 700? -> test it.
#include <hip/hip_runtime.h>
#include <cstdio>
#include <cstdlib>
__device__ float s[32];                       // <-- GLOBAL device memory (not shared)
__device__ float explodePlease(float *f, int i){ return f[1 - i]; }
__global__ void kernel(float *g, int i, bool b){
    s[threadIdx.x]=0; __syncthreads();
    *g = explodePlease(b ? s : g, i);         // b=1 -> use global s ; b=0 -> use hipMalloc g
}
int main(int c,char**v){int i=atoi(v[1]);bool b=atoi(v[2]);
  float* g; hipMalloc(&g,sizeof(float)*64); hipMemset(g,0,sizeof(float)*64);
  kernel<<<1,32>>>(g,i,b);
  hipError_t e=hipDeviceSynchronize();
  printf("i=%d b=%d(%s) -> CODE=%d (%s)\n", i,b, b?"global s":"hipMalloc g",
         (int)e, hipGetErrorString(e));
  return 0; }
