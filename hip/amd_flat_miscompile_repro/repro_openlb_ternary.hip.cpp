// OpenLB-shape accessor: TWO GLOBAL pointers selected by a ternary.
//   start0 = base + shift          (in range)
//   start1 = base - (count-shift)  (points BEFORE the array = UB, like OpenLB)
//   read (i > remainder) ? start1[i] : start0[i]  ==  base[(shift+i) % count]
// All GLOBAL (no __shared__), exactly like OpenLB's cyclic Branching accessor.
// Optionally force generic/flat with b?g:g2 to mimic pointer-provenance mixing.
#include <hip/hip_runtime.h>
#include <cstdio>
__global__ void kern(const float* base, int count, int shift,
                     const int* idx, float* out, int n){
    int t = blockIdx.x*blockDim.x + threadIdx.x; if(t>=n) return;
    const float* start0 = base + shift;
    const float* start1 = base - (count - shift);   // BEFORE array (UB)
    long remainder = (long)(count - shift) - 1;      // matches OpenLB sign convention
    int i = idx[t];                                  // i in [0,count)
    out[t] = ((long)i > remainder) ? start1[i] : start0[i];  // == base[(shift+i)%count]
}
int main(){
  const int count = 1<<20, shift = 300007, n = count;
  float* base; hipMalloc(&base, count*sizeof(float));
  { float* h=(float*)malloc(count*sizeof(float)); for(int j=0;j<count;++j)h[j]=(float)j;
    hipMemcpy(base,h,count*sizeof(float),hipMemcpyHostToDevice); free(h);}
  int* didx; hipMalloc(&didx,n*sizeof(int));
  { int* h=(int*)malloc(n*sizeof(int)); for(int t=0;t<n;++t)h[t]=t;
    hipMemcpy(didx,h,n*sizeof(int),hipMemcpyHostToDevice); free(h);}
  float* dout; hipMalloc(&dout,n*sizeof(float));
  kern<<<(n+255)/256,256>>>(base,count,shift,didx,dout,n);
  hipError_t se=hipDeviceSynchronize();
  printf("sync: %s\n", hipGetErrorString(se));
  if(se==hipSuccess){
    float* h=(float*)malloc(n*sizeof(float));
    hipMemcpy(h,dout,n*sizeof(float),hipMemcpyDeviceToHost);
    int bad=0; for(int t=0;t<n;++t){ float ref=(float)((shift+(long)t)%count);
      if(h[t]!=ref){ if(bad<3)printf("  MISMATCH t=%d got=%.1f ref=%.1f\n",t,h[t],ref); ++bad; } }
    printf("mismatches=%d %s\n", bad, bad?"<<< MISCOMPILE":"(all correct)");
    free(h);
  } else printf("<<< CRASH\n");
  return 0;
}
