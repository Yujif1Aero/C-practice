#include <hip/hip_runtime.h>
#include <cstdio>
#include <cstdlib>
__device__ float explodePlease(float *f, int i){ return f[1 - i]; }
__global__ void kernel(float *g, int i, bool b){
    __shared__ float s[32]; s[threadIdx.x]=0; __syncthreads();
    *g = explodePlease(b ? s : g, i);
}
int main(int c,char**v){int i=atoi(v[1]);bool b=atoi(v[2]);
  float* g; hipMalloc(&g,256); hipMemset(g,0,256);
  kernel<<<1,32>>>(g,i,b);
  hipError_t e=hipDeviceSynchronize();
  printf("i=%d b=%d -> HIP error CODE=%d  (%s)\n", i,b,(int)e, hipGetErrorString(e));
  return e; }
