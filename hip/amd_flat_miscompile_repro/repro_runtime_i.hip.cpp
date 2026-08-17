// THE climax test. #4389 form: f[C - i], C = compile-time constant, i = RUNTIME.
//   base' (flat_load base register) = f - i   (RUNTIME => compiler CANNOT cancel => stays OUT of range for i>0)
//   offset:C folded (compile-time constant)
//   final = f[C - i]  => IN range for 0<=i<=C
// If it faults / misreads while final is in-range => genuine -O3 miscompile.
#include <hip/hip_runtime.h>
#include <cstdio>
template<int C>
__global__ void kern(float* g, int i, bool b, float* out){
    __shared__ float s[64]; s[threadIdx.x]=0; __syncthreads();
    float* f = (b ? s : g);     // flat force
    *out = f[C - i];            // base' = f - i (runtime), offset:C, final = f[C-i]
}
int main(){
  const long N = 1<<20;
  float* g; hipMalloc(&g, N*sizeof(float));
  float* out; hipMalloc(&out, sizeof(float));
  { float* h=(float*)malloc(N*sizeof(float)); for(long j=0;j<N;++j) h[j]=(float)j;
    hipMemcpy(g,h,N*sizeof(float),hipMemcpyHostToDevice); free(h);}
  const int C = 1023;                 // constant (foldable into flat offset, <=~4KB)
  int firstFault=-1, firstWrong=-1;
  for(int i=0;i<=C;++i){              // final index = C-i in [0,C] => IN range
    float got=-1; kern<C><<<1,64>>>(g,i,false,out);
    hipError_t se=hipDeviceSynchronize();
    if(se!=hipSuccess){ if(firstFault<0){firstFault=i;
        printf("FAULT at i=%d : base'=g-%d (OUT), final=g[%d] (IN range) : %s\n",
               i,i,C-i,hipGetErrorString(se)); } break; }
    hipMemcpy(&got,out,sizeof(float),hipMemcpyDeviceToHost);
    if(got!=(float)(C-i) && firstWrong<0){ firstWrong=i;
       printf("WRONG at i=%d : got=%.1f expect=%.1f (final in range)\n",i,got,(float)(C-i)); }
  }
  if(firstFault<0 && firstWrong<0) printf("all correct: no fault/wrong for i in [0,%d]\n",C);
  return firstFault>=0||firstWrong>=0;
}
