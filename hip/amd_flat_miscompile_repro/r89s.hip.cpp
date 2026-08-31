#include <hip/hip_runtime.h>
#include <cstdio>
#include <cstdlib>
__device__ float explodePlease(float *f, int i){ return f[1 - i]; }
__global__ void kernel(float *g, int i, bool b){
    __shared__ float s[32];
    s[threadIdx.x] = 0; __syncthreads();
    *g = explodePlease(b ? s : g, i);
}
int main(int argc,char**argv){
  int i=atoi(argv[1]); bool b=atoi(argv[2]);
  float* g; hipMalloc(&g,sizeof(float)*64); hipMemset(g,0,sizeof(float)*64);
  kernel<<<1,32>>>(g,i,b);
  hipError_t se=hipDeviceSynchronize();
  printf("b=%d i=%d : %s\n", b,i, hipGetErrorString(se));
  return se!=hipSuccess;
}
