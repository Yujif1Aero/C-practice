// The ACTUAL #4389 condition: b=true (pointer is __shared__ s), i=0
//   base = s + (1 - 0) - 1elem ... i.e. flat base = s - 1elem = 4 bytes BEFORE the SHARED aperture
//   -> hardware misclassifies the memory space -> crash. FINAL addr (s[1]) is in range!
#include <hip/hip_runtime.h>
#include <cstdio>
__device__ float explodePlease(float *f, int i){ return f[1 - i]; }
__global__ void kernel(float *g, int i, bool b){
    __shared__ float s[32];
    s[threadIdx.x] = 0; __syncthreads();
    *g = explodePlease(b ? s : g, i);
}
int main(){
  float* g; hipMalloc(&g,sizeof(float)*64); hipMemset(g,0,sizeof(float)*64);
  struct C{int i;bool b;const char*note;};
  C cases[]={{0,true,"shared, base=s-0? final s[1] in-range (THE #4389 case)"},
             {0,false,"global, final g[1] in-range"},
             {1,true,"shared, final s[0] in-range, base=s-1 (before shared aperture)"},
             {1,false,"global, final g[0] in-range, base=g-1"}};
  for(auto c:cases){
    kernel<<<1,32>>>(g,c.i,c.b);
    hipError_t se=hipDeviceSynchronize();
    printf("b=%d i=%d : %-55s : %s %s\n", c.b,c.i,c.note, hipGetErrorString(se),
           se!=hipSuccess?"  <<< CRASH (final in-range => MISCOMPILE)":"");
  }
  return 0;
}
