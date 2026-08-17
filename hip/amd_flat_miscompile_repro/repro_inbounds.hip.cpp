// Cleaner test of "is it a MISCOMPILE or just OOB?"
//
// The minimal #4389 case f[1-i] at i=2 reads f[-1] = a GENUINELY out-of-bounds
// address, so faulting proves nothing. A real miscompile means:
//   the FINAL (effective) address is IN BOUNDS, yet the code faults/misreads
//   because the compiler folds a FAR-before-array base into the flat_load and
//   the hardware decodes the memory space from that (invalid) base.
//
// Here: base = f - M  (M elements before the array, like OpenLB start1=base-(N-shift)),
// then base[idx] with idx = M + k  =>  final = f[k], k in [0,N) = IN BOUNDS.
// b?s:g forces a flat (generic) pointer so we get flat_load (not global_load).
// We sweep M from small (same page) to large (MB before => different region).
//   fault while final is in-bounds  => MISCOMPILE-class (base used for decode)
//   correct value                   => no miscompile at this M
#include <hip/hip_runtime.h>
#include <cstdio>

__device__ float loadAt(float* base, long idx){ return base[idx]; }

__global__ void kernel(float* g, long M, long k, bool b, float* out){
    __shared__ float s[64];
    s[threadIdx.x] = 0.0f;
    __syncthreads();
    float* f    = (b ? s : g);   // force generic/flat pointer
    float* base = f - M;         // pointer FAR before the array (UB), like start1
    *out = loadAt(base, M + k);  // final = f[k]  (IN BOUNDS for k in [0,N))
}

int main(){
  const long N = 1<<20;                 // 1M floats = 4 MB buffer
  float* g;   if(hipMalloc(&g, N*sizeof(float))!=hipSuccess){printf("malloc\n");return 2;}
  float* out; hipMalloc(&out, sizeof(float));
  // init g[k] = k so we can check the value actually read
  { float* h=(float*)malloc(N*sizeof(float)); for(long i=0;i<N;++i) h[i]=(float)i;
    hipMemcpy(g,h,N*sizeof(float),hipMemcpyHostToDevice); free(h); }

  long Ms[] = {1, 16, 1024, 4096, 65536, 262144, 1048575}; // elems before array
  const long k = 7;                     // final index (in bounds): expect value 7
  for(bool b : {false}){                // b=false => f=g (global); flat forced by the select
    for(long M : Ms){
      float got = -12345.0f;
      kernel<<<1,64>>>(g, M, k, b, out);
      hipError_t se = hipDeviceSynchronize();
      if(se==hipSuccess) hipMemcpy(&got,out,sizeof(float),hipMemcpyDeviceToHost);
      bool faulted = (se!=hipSuccess);
      bool wrong   = (!faulted && got!=(float)k);
      printf("M=%7ld (base = g - %ld elems = %ld bytes before) : %s  got=%.1f expect=%.1f  %s\n",
             M, M, M*4, hipGetErrorString(se), got, (float)k,
             faulted? "<<< FAULT (base-decode, final was in-bounds => MISCOMPILE-class)" :
             wrong  ? "<<< WRONG VALUE (=> MISCOMPILE-class)" : "ok(correct)");
    }
  }
  return 0;
}
