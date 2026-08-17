// Experiment C: base OUT of range, offset is a COMPILE-TIME CONSTANT, final IN range.
// This is the condition where -O3 CAN fold a constant into flat_load offset:K
// while keeping the out-of-range base in the register. Does -O3 then fault?
#include <hip/hip_runtime.h>
#include <cstdio>
template<int M, int OFF>
__global__ void kern(float* g, bool b, float* out){
    __shared__ float s[64]; s[threadIdx.x]=0; __syncthreads();
    float* f    = (b ? s : g);   // flat force
    float* base = f - M;         // base = f - M  (M elems BEFORE array => OUT of range)
    *out = base[OFF];            // final = f + (OFF - M).  Pick OFF=M+7 => final = f[7] IN range
}
int main(){
  const long N=1<<20; float* g; hipMalloc(&g,N*sizeof(float));
  float* out; hipMalloc(&out,sizeof(float));
  { float* h=(float*)malloc(N*sizeof(float)); for(long i=0;i<N;++i)h[i]=(float)i;
    hipMemcpy(g,h,N*sizeof(float),hipMemcpyHostToDevice); free(h);}
  auto go=[&](const char* tag, auto kern){
    float got=-1; kern<<<1,64>>>(g,false,out);
    hipError_t se=hipDeviceSynchronize();
    if(se==hipSuccess) hipMemcpy(&got,out,sizeof(float),hipMemcpyDeviceToHost);
    printf("%-22s : %s  got=%.1f expect=7.0 %s\n", tag, hipGetErrorString(se), got,
      se!=hipSuccess?"<<<FAULT":(got!=7.0f?"<<<WRONG":"ok"));
  };
  go("M=64  OFF=71",   kern<64,   71>);      // base 256 B before,  final f[7]
  go("M=1024 OFF=1031",kern<1024, 1031>);    // base 4 KB before,   final f[7]
  go("M=65536 OFF=65543",kern<65536,65543>); // base 256 KB before, final f[7]
  return 0;
}
